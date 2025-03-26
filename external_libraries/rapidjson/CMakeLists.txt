MESSAGE(STATUS "rapidjson: true")
add_library(rapidjson INTERFACE)
target_include_directories(spdlog
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    INTERFACE $<INSTALL_INTERFACE:include>
)