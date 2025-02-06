#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>

#include <adios2.h>
#include <mpi.h>

#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "../../gray-scott/common/timer.hpp"
#include "../../gray-scott/simulation/gray-scott.h"
#include "../../gray-scott/simulation/restart.h"
#include "../../gray-scott/simulation/writer.h"

void print_io_settings(const adios2::IO &io)
{
    std::cout << "Simulation writes data using engine type:              "
              << io.EngineType() << std::endl;
    auto ioparams = io.Parameters();
    std::cout << "IO parameters:  " << std::endl;
    for (const auto &p : ioparams)
    {
        std::cout << "    " << p.first << " = " << p.second << std::endl;
    }
}

void print_settings(const Settings &s, int restart_step)
{
    std::cout << "grid:             " << s.L << "x" << s.L << "x" << s.L
              << std::endl;
    if (restart_step > 0)
    {
        std::cout << "restart:          from step " << restart_step
                  << std::endl;
    }
    else
    {
        std::cout << "restart:          no" << std::endl;
    }
    std::cout << "steps:            " << s.steps << std::endl;
    std::cout << "plotgap:          " << s.plotgap << std::endl;
    std::cout << "F:                " << s.F << std::endl;
    std::cout << "k:                " << s.k << std::endl;
    std::cout << "dt:               " << s.dt << std::endl;
    std::cout << "Du:               " << s.Du << std::endl;
    std::cout << "Dv:               " << s.Dv << std::endl;
    std::cout << "noise:            " << s.noise << std::endl;
    std::cout << "output:           " << s.output << std::endl;
    std::cout << "adios_config:     " << s.adios_config << std::endl;
}

void print_simulator_settings(const GrayScott &s)
{
    std::cout << "process layout:   " << s.npx << "x" << s.npy << "x" << s.npz
              << std::endl;
    std::cout << "local grid size:  " << s.size_x << "x" << s.size_y << "x"
              << s.size_z << std::endl;
}

int main(int argc, char **argv)
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    int rank, procs, wrank;

    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

    const unsigned int color = 1;
    MPI_Comm comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, wrank, &comm);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &procs);

    // Create the logger and set up console and file sinks
    std::string binaryDir = BINARY_DIR;

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern("%v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(binaryDir + "/logs/sim_test.txt", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("%v");

    spdlog::logger logger("debug_logger", { console_sink, file_sink });
    logger.set_level(spdlog::level::debug);

    if (argc < 2)
    {
        if (rank == 0)
        {
            std::cerr << "Too few arguments" << std::endl;
            std::cerr << "Usage: gray-scott settings.json" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    Timer gs_setting_up("gs_setting_up", true);
    Settings settings = Settings::from_json(argv[1]);

    bool derived = atoi(argv[2]);


    GrayScott sim(settings, comm);
    gs_setting_up.print_csv();
    Timer gs_init_up("gs_init_up", true);
    sim.init();
    gs_init_up.print_csv();

    adios2::ADIOS adios(settings.adios_config, comm);
    adios2::IO io_main = adios.DeclareIO("SimulationOutput");
    adios2::IO io_ckpt = adios.DeclareIO("SimulationCheckpoint");
    int restart_step = 0;
    if (settings.restart)
    {
        restart_step = ReadRestart(comm, settings, sim, io_ckpt);

        io_main.SetParameter("AppendAfterSteps",
                             std::to_string(restart_step / settings.plotgap));
    }

    Writer writer_main(settings, sim, io_main, derived);

    writer_main.open(settings.output, (restart_step > 0));

    auto app_start_time = std::chrono::high_resolution_clock::now(); // Record end time of the application

    if (rank == 0)
    {
        print_io_settings(io_main);
        std::cout << "========================================" << std::endl;
        print_settings(settings, restart_step);
        print_simulator_settings(sim);
        std::cout << "========================================" << std::endl;
    }



    for (int it = restart_step; it < settings.steps;)
    {

        Timer gs_loop("gs_loop", true);  // Starts immediately
        Timer gs_computation("gs_computation", true);
        sim.iterate();
        gs_computation.print_csv();
        it++;



        if (it % settings.plotgap == 0)
        {
            if (rank == 0)
            {
                std::cout << "Simulation at step " << it
                          << " writing output step     "
                          << it / settings.plotgap << std::endl;
            }

            writer_main.write(it, sim, rank);
        }

        if (settings.checkpoint && (it % settings.checkpoint_freq) == 0)
        {
            WriteCkpt(comm, it, settings, sim, io_ckpt);
        }

        gs_loop.print_csv();

    }
    Timer gs_close("gs_close", true);
    writer_main.close();
    gs_close.print_csv();

    MPI_Barrier(comm);
    gs_loop.print_csv();
    auto app_end_time = std::chrono::high_resolution_clock::now(); // Record end time of the application
    auto app_duration = std::chrono::duration_cast<std::chrono::milliseconds>(app_end_time - app_start_time);
    logger.info("Rank {} - ET {} - milliseconds", rank, app_duration.count());

    MPI_Finalize();
}
