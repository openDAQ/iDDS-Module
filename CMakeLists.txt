cmake_minimum_required(VERSION 3.25)
set(REPO_NAME idds_module)
set(REPO_OPTION_PREFIX IDDS)

project(${REPO_NAME} VERSION 1.0.0)

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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(${REPO_OPTION_PREFIX}_ENABLE_EXAMPLE "Enable Examples" ON)

list(APPEND CMAKE_MESSAGE_CONTEXT ${REPO_NAME})
set(CMAKE_MESSAGE_CONTEXT_SHOW ON CACHE BOOL "Show CMake message context")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CommonUtils)
setup_repo(${REPO_OPTION_PREFIX})

include(SetupDDS)
setup_dds_dependencies()

add_subdirectory(external)
add_subdirectory(idds_wrapper)
#add_subdirectory(idds_server_module)
#add_subdirectory(idds_client_module)
add_subdirectory(helloworld)

if (${REPO_OPTION_PREFIX}_ENABLE_EXAMPLE)
    add_subdirectory(examples)
endif()