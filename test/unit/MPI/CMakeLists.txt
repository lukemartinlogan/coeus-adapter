add_executable(mpi_test mpit_tests.cpp)

target_link_libraries(mpi_test PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        MPI::MPI_CXX
        hermes::hermes_core_client
        ${ADIOS2_LIBRARIES}
        spdlog
        hermes::hermes_core_client
        rapidjson)

target_include_directories(mpi_test PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${ADIOS2_INCLUDE_DIR})