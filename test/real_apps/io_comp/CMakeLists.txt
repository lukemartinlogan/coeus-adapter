add_executable(io_comp io_comp.cpp)
target_link_libraries(io_comp PRIVATE
        ${ADIOS2_LIBRARIES}
        spdlog
        hermes::core_client
        rapidjson
        MPI::MPI_CXX)
target_include_directories(io_comp PRIVATE
        ${ADIOS2_INCLUDE_DIR})