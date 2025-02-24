cmake_minimum_required(VERSION 3.25)
set(REPO_NAME idds_module)
set(REPO_OPTION_PREFIX IDDS)

project(${REPO_NAME} VERSION 1.0.0)

# Set iDDS Modules variables

set(${REPO_OPTION_PREFIX}_ENABLE_EXAMPLE "Enable Examples" ON)
set(${REPO_OPTION_PREFIX}_ENABLE_TESTS   "Enable Tests" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set OpenDAQ variables
set(DAQMODULES_REF_FB_MODULE OFF)
set(OPENDAQ_ENABLE_OPCUA OFF)
set(DAQMODULES_EMPTY_MODULE OFF)
set(DAQMODULES_OPENDAQ_CLIENT_MODULE OFF)
set(DAQMODULES_OPENDAQ_SERVER_MODULE OFF)
set(DAQMODULES_REF_DEVICE_MODULE ON)
set(DAQMODULES_AUDIO_DEVICE_MODULE OFF)
set(OPENDAQ_ENABLE_WEBSOCKET_STREAMING OFF)
set(APP_ENABLE_WEBPAGE_EXAMPLES OFF)
set(OPENDAQ_DOCUMENTATION_TESTS OFF)
set(OPENDAQ_ENABLE_NATIVE_STREAMING OFF)



IF (WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
ENDIF(WIN32)

list(APPEND CMAKE_MESSAGE_CONTEXT ${REPO_NAME})
set(CMAKE_MESSAGE_CONTEXT_SHOW ON CACHE BOOL "Show CMake message context")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CommonUtils)
setup_repo(${REPO_OPTION_PREFIX})

include(SetupDDS)
setup_dds_dependencies()

add_subdirectory(external)
add_subdirectory(idds_wrapper)
add_subdirectory(idds_server_module)
add_subdirectory(idds_client_module)
add_subdirectory(idds_xml_parser)

if (${REPO_OPTION_PREFIX}_ENABLE_EXAMPLE)
    add_subdirectory(examples)
endif()

# Set CPack variables
set(CPACK_COMPONENTS_ALL RUNTIME)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/package")

# Set the CPack generator based on the platform
if (WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif (UNIX AND NOT APPLE)
    cmake_host_system_information(RESULT DISTRO_ID QUERY DISTRIB_ID)
    cmake_host_system_information(RESULT DISTRO_VERSION_ID QUERY DISTRIB_VERSION_ID)
    set(CPACK_SYSTEM_NAME "${DISTRO_ID}${DISTRO_VERSION_ID}")
    set(CPACK_GENERATOR "TGZ")
endif()

# Include CPack for packaging
include(CPack)
