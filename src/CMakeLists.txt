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

#
# #-----------------------------------------------------------------------------
# # Add Target(s) to CMake Install for import into other projects
# #-----------------------------------------------------------------------------
# install(
# EXPORT
# ${CHIMAERA_EXPORTED_TARGETS}
# DESTINATION
# ${COEUS_INSTALL_DATA_DIR}/cmake/COEUS
# FILE
# ${CHIMAERA_EXPORTED_TARGETS}.cmake
# )

# ------------------------------------------------------------------------------
# Set variables for parent scope
# ------------------------------------------------------------------------------
# Used by config.cmake.build.in and Testing
set(COEUS_INCLUDES_BUILD_TIME
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${COEUS_EXT_INCLUDE_DEPENDENCIES}
  PARENT_SCOPE)

# Used by config.cmake.install.in
set(COEUS_INCLUDES_INSTALL_TIME
  ${COEUS_INSTALL_INCLUDE_DIR}
  ${COEUS_EXT_INCLUDE_DEPENDENCIES}
  PARENT_SCOPE)
