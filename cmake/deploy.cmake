include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "Install prefix" FORCE)
endif()

set(INSTALL_RB_HOME "${CMAKE_INSTALL_INCLUDEDIR}/rb")
set(CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/RbConfig.cmake")
set(CONFIG_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/RbConfigVersion.cmake")
set(CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Rb")

configure_package_config_file(
    "${RB_HOME}/RbConfig.cmake.in"
    "${CONFIG_FILE}"
    INSTALL_DESTINATION "${CONFIG_DESTINATION}")

write_basic_package_version_file("${CONFIG_VERSION_FILE}"
    COMPATIBILITY AnyNewerVersion)

install(TARGETS ${LIB}
    EXPORT RbTargets
    INCLUDES DESTINATION "${INSTALL_RB_HOME}")

install(EXPORT RbTargets
    NAMESPACE Rb::
    DESTINATION "${CONFIG_DESTINATION}")

install(FILES "${CONFIG_FILE}" "${CONFIG_VERSION_FILE}"
    DESTINATION "${CONFIG_DESTINATION}")

install(FILES "${VERSION_FILE}" "${FEATURE_FILE}"
    DESTINATION "${INSTALL_RB_HOME}/rb")

# setting the PUBLIC_HEADER property does not preserve the folder structure
# so instead we don't use it, iterate through public headers and install them manually
foreach(HEADER ${HEADERS})
    file(RELATIVE_PATH HEADER_FILE_PATH "${RB_HOME}" "${HEADER}")
    get_filename_component(HEADER_DIRECTORY_PATH "${HEADER_FILE_PATH}" DIRECTORY)
    install(FILES ${HEADER}
        DESTINATION "${INSTALL_RB_HOME}/${HEADER_DIRECTORY_PATH}")
endforeach()
