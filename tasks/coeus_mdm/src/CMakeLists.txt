# ------------------------------------------------------------------------------
# Set variables
# ------------------------------------------------------------------------------
set(MOD_EXPORTS ${REPO_NAMESPACE}_coeus_mdm_exports)

# ------------------------------------------------------------------------------
# Build coeus_mdm Task Library
# ------------------------------------------------------------------------------
add_chimod_runtime_lib(${REPO_NAMESPACE} coeus_mdm coeus_mdm_runtime.cc)
add_chimod_client_lib(${REPO_NAMESPACE} coeus_mdm coeus_mdm_client.cc)

foreach(target IN LISTS ${REPO_NAMESPACE}_coeus_mdm_runtime_iter)
        target_link_libraries(${target} PUBLIC
                ${SQLite3_LIBRARIES})
        target_include_directories(${target} PRIVATE
                ${PROJECT_SOURCE_DIR}/include
                ${SQLite3_INCLUDE_DIRS})
        message("target: ${target}")
endforeach()

# ------------------------------------------------------------------------------
# Install coeus_mdm Task Library
# ------------------------------------------------------------------------------
install(
        TARGETS
        ${${MOD_EXPORTS}}
        EXPORT
        ${CHIMAERA_EXPORTED_TARGETS}
        LIBRARY DESTINATION ${CHIMAERA_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${CHIMAERA_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${CHIMAERA_INSTALL_BIN_DIR}
)
install(FILES coeus_mdm_monitor.py
        DESTINATION ${CHIMAERA_INSTALL_LIB_DIR})

# ------------------------------------------------------------------------------
# Coverage
# ------------------------------------------------------------------------------
if(CHIMAERA_ENABLE_COVERAGE)
        set_coverage_flags(coeus_mdm)
endif()
