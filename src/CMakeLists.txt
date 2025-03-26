include_directories(${CMAKE_SOURCE_DIR})

# ------------------------------------------------------------------------------
# Set sources
# ------------------------------------------------------------------------------
set(HERMES_ENGINE_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/hermes_engine.cc)

# ------------------------------------------------------------------------------
# Libraries
# ------------------------------------------------------------------------------
add_library(hermes_engine ${HERMES_ENGINE_SRCS})
add_dependencies(hermes_engine spdlog)
add_dependencies(hermes_engine coeus_mdm)
add_dependencies(hermes_engine rankConsensus)
target_include_directories(hermes_engine PUBLIC
  ${ADIOS2_INCLUDE_DIR}
  ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(hermes_engine LINK_PUBLIC
  hermes::hermes_core_client
  ${ADIOS2_LIBRARIES}
  spdlog
  rapidjson
  ${SQLite3_LIBRARIES}
  coeus::coeus_mdm_client
  coeus::rankConsensus_client
)

# -----------------------------------------------------------------------------
# Add Target(s) to CMake Install
# -----------------------------------------------------------------------------
install(
  TARGETS
  hermes_engine
  spdlog
  rapidjson
  EXPORT
  ${CHIMAERA_EXPORTED_TARGETS}
  LIBRARY DESTINATION ${COEUS_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${COEUS_INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${COEUS_INSTALL_BIN_DIR}
)
