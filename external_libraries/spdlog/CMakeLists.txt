MESSAGE(STATUS "SPDLOG: true")
find_package(Threads REQUIRED)
add_library(spdlog INTERFACE)
target_include_directories(spdlog
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    INTERFACE $<INSTALL_INTERFACE:include>
)
target_link_libraries(spdlog INTERFACE Threads::Threads)
