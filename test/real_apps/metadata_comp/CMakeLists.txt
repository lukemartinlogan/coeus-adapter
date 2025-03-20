add_executable(metadata_hermes metadata_hermes.cpp)
target_include_directories(metadata_hermes PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${ADIOS2_INCLUDE_DIR})
target_link_libraries(metadata_hermes PRIVATE
        hermes::core_client
        ${ADIOS2_LIBRARIES}
        spdlog
        hermes::core_client
        rapidjson)

# add_executable(metadata_empress metadata_empress.cpp)
# target_include_directories(metadata_empress PRIVATE
# ${PROJECT_SOURCE_DIR}/include
# ${ADIOS2_INCLUDE_DIR}
# ${SQLite3_INCLUDE_DIRS}
# )
# target_link_libraries(metadata_empress PRIVATE
# hermes::core_client
# ${ADIOS2_LIBRARIES}
# spdlog
# rapidjson
# ${SQLite3_LIBRARIES}
# )
