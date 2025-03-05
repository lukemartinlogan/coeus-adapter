/*
 * Analysis code for the Gray-Scott application.
 * Reads variable U and V, and computes the PDF for each 2D slices of U and V.
 * Writes the computed PDFs using ADIOS.
 *
 * Norbert Podhorszki, pnorbert@ornl.gov
 *
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "adios2.h"
#include <mpi.h>
#include "../../gray-scott/common/timer.hpp"

std::string concatenateVectorToString(const std::vector<size_t> &vec) {
    std::stringstream ss;
    ss << "( ";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i < vec.size() - 1) {
            ss << ", ";
        }
    }
    ss << " )";
    return ss.str();
}


/*
 * Print info to the user on how to invoke the application
 */
void printUsage()
{
    std::cout
            << "Usage: pdf_calc input output [N] [output_inputdata]\n"
            << "  input:   Name of the input file handle for reading data\n"
            << "  output:  Name of the output file to which data must be written\n"
            << "  N:       Number of bins for the PDF calculation, default = 1000\n"
            << "  output_inputdata: YES will write the original variables besides "
               "the analysis results\n\n";
}

/*
 * MAIN
 */
int main(int argc, char *argv[])
{
    auto app_start_time = std::chrono::high_resolution_clock::now();
    MPI_Init(&argc, &argv);
    int rank, comm_size, wrank;
    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
    const unsigned int color = 2;
    MPI_Comm comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, wrank, &comm);
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);
    Timer gs_hashing_setting_time("gs_hashing_setting_time" , true);
    std::string in_filename;
    std::string out_filename;
    bool write_inputvars = true;
    in_filename = argv[1];
    out_filename = argv[2];




    // set up dataset size
    std::size_t u_global_size, v_global_size;
    std::size_t u_local_size, v_local_size;
    bool firstStep = true;
    std::vector<std::size_t> shape;
    size_t count1;
    size_t start1;
    std::vector<double> u;
    std::vector<double> v;
    //
    int simStep = -5;
    // adios2 variable declarations
    adios2::Variable<double> var_u_in, var_v_in;
    adios2::Variable<int> var_step_in;
    adios2::Variable<int> var_step_out;
    adios2::Variable<double> var_u_out, var_v_out;
    adios2::ADIOS ad("adios2.xml", comm);
    // IO objects for reading and writing
    adios2::IO reader_io = ad.DeclareIO("SimulationOutput");
    adios2::IO writer_io = ad.DeclareIO("PDFAnalysisOutput");

    if (!rank)
    {
        std::cout << "PDF analysis reads from Simulation using engine type:  "
                  << reader_io.EngineType() << std::endl;
        std::cout << "PDF analysis writes using engine type:                 "
                  << writer_io.EngineType() << std::endl;
    }
    // Engines for reading and writing
    adios2::Engine reader =
            reader_io.Open(in_filename, adios2::Mode::Read, comm);
    adios2::Engine writer =
            writer_io.Open(out_filename, adios2::Mode::Write, comm);
    bool shouldIWrite = (!rank || reader_io.EngineType() == "HDF5");
    // timer 1
    gs_hashing_setting_time.print_csv();
    // read data step-by-step
    int stepAnalysis = 0;

    while (true)
    {
        adios2::StepStatus read_status =
                reader.BeginStep(adios2::StepMode::Read, 10.0f);
        if (read_status == adios2::StepStatus::NotReady)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        else if (read_status != adios2::StepStatus::OK)
        {
            break;
        }
        // int stepSimOut = reader.CurrentStep();
        int stepSimOut = stepAnalysis;
        // Inquire variable
        var_u_in = reader_io.InquireVariable<double>("U");
        var_v_in = reader_io.InquireVariable<double>("V");
        var_step_in = reader_io.InquireVariable<int>("step");

        // Set the selection at the first step only, assuming that
        // the variable dimensions do not change across timesteps
        if (firstStep) {
            Timer gs_hashing_first_step_time("gs_hashing_first_step_time" , true);
            shape = var_u_in.Shape();
            // Calculate global and local sizes of U and V
            u_global_size = shape[0] * shape[1] * shape[2];
            u_local_size = u_global_size / comm_size;
            v_global_size = shape[0] * shape[1] * shape[2];
            v_local_size = v_global_size / comm_size;
            // 1D decomposition
            count1 = shape[0] / comm_size;
            start1 = count1 * rank;
            if (rank == comm_size - 1) {
                // last process need to read all the rest of slices
                count1 = shape[0] - count1 * (comm_size - 1);
            }
            if (shouldIWrite) {
            }

            if (write_inputvars) {
                var_u_out = writer_io.DefineVariable<double>(
                        "U", {shape[0], shape[1], shape[2]}, {start1, 0, 0},
                        {count1, shape[1], shape[2]});
                var_v_out = writer_io.DefineVariable<double>(
                        "V", {shape[0], shape[1], shape[2]}, {start1, 0, 0},
                        {count1, shape[1], shape[2]});
                auto PDFU = writer_io.DefineDerivedVariable("derive/hashU",
                                                            "x = U \n"
                                                            "hash(x)",
                                                            adios2::DerivedVarType::StoreData);

                auto PDFV = writer_io.DefineDerivedVariable("derive/hashV",
                                                            "x = V \n"
                                                            "hash(x)",
                                                            adios2::DerivedVarType::StoreData);
            }
            firstStep = false;
            gs_hashing_first_step_time.print_csv();
        }
        var_u_in.SetSelection(adios2::Box<adios2::Dims>(
                {start1, 0, 0}, {count1, shape[1], shape[2]}));
        var_v_in.SetSelection(adios2::Box<adios2::Dims>(
                {start1, 0, 0}, {count1, shape[1], shape[2]}));

        // Read adios2 data
        Timer gs_hashing_get_variable_u("gs_hashing_get_variable_u" , true);

        //reader.Get<double>(var_u_in, u, adios2::Mode::Sync);
        reader.Get<double>(var_u_in, u, );

        gs_hashing_get_variable_u.print_csv();
        Timer gs_hashing_get_variable_v("gs_hashing_get_variable_v" , true);

        //reader.Get<double>(var_v_in, v, adios2::Mode::Sync);
        reader.Get<double>(var_v_in, v);


        //reader.Get<double>(var_v_in, v, adios2::Mode::Sync);
        gs_hashing_get_variable_v.print_csv();
        std::cout << "Get U: " << rank << " size: " << u.size()
                  << " Count: (" << concatenateVectorToString(var_u_in.Count()) << ") "
                  << " Start: (" << concatenateVectorToString(var_u_in.Start()) << ") "
                  << " Shape: (" << concatenateVectorToString(var_u_in.Shape()) << ") "
                  << std::endl;
        std::cout << "Get V: " << rank << " size: " << v.size()
                  << " Count: (" << concatenateVectorToString(var_v_in.Count()) << ") "
                  << " Start: (" << concatenateVectorToString(var_v_in.Start()) << ") "
                  << " Shape: (" << concatenateVectorToString(var_v_in.Shape()) << ") "
                  << std::endl;
        if (shouldIWrite)
        {
            std::cout << "Get step: " << rank << std::endl;
            reader.Get<int>(var_step_in, &simStep);
        }
        // End read step (let resources about step go)
        Timer gs_hashing_reader_endStep("gs_hashing_reader_endStep" , true);
        reader.EndStep();
        gs_hashing_reader_endStep.print_csv();
        if (!rank)
        {
            std::cout << "PDF Analysis step " << stepAnalysis
                      << " processing sim output step " << stepSimOut
                      << " sim compute step " << simStep << std::endl;
        }
        Timer gs_hashing_writer_beginStep("gs_hashing_writer_beginStep" , true);
        writer.BeginStep();
        gs_hashing_writer_beginStep.print_csv();
        Timer gs_hashing_writer_put_u("gs_hashing_writer_put_u" , true);
        writer.Put<double>(var_u_out, u.data());
        //writer.Put<double>(var_u_out, u.data(), adios2::Mode::Sync);
        gs_hashing_writer_put_u.print_csv();
        Timer gs_hashing_writer_put_v("gs_hashing_writer_put_v" , true);
        //writer.Put<double>(var_v_out, v.data(), adios2::Mode::Sync);
        writer.Put<double>(var_v_out, v.data());
        gs_hashing_writer_put_v.print_csv();
        Timer gs_hashing_writer_endstep("gs_hashing_writer_endstep" , true);
        writer.EndStep();
        gs_hashing_writer_endstep.print_csv();
        ++stepAnalysis;
    }

    // cleanup (close reader and writer)
    Timer gs_hashing_reader_close("gs_hashing_reader_close" , true);
    reader.Close();
    gs_hashing_reader_close.print_csv();
    Timer gs_hashing_writer_close("gs_hashing_writer_close" , true);
    writer.Close();
    gs_hashing_writer_close.print_csv();
    auto app_end_time = std::chrono::high_resolution_clock::now(); // Record end time of the application
    auto app_duration = std::chrono::duration_cast<std::chrono::milliseconds>(app_end_time - app_start_time);

    std::cout << "rank:" << rank << ", app duration time: " <<  app_duration.count() << std::endl;
    MPI_Barrier(comm);
    MPI_Finalize();
    return 0;
}
