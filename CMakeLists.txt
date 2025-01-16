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
set(${REPO_OPTION_PREFIX}_ENABLE_EXAMPLE "Enable Example" ON)

list(APPEND CMAKE_MESSAGE_CONTEXT ${REPO_NAME})
set(CMAKE_MESSAGE_CONTEXT_SHOW ON CACHE BOOL "Show CMake message context")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CommonUtils)
setup_repo(${REPO_OPTION_PREFIX})

if (${REPO_OPTION_PREFIX}_ENABLE_EXAMPLE)
    #add_subdirectory(examples)
endif()

add_subdirectory(external)
#add_subdirectory(idds_wrapper)
#add_subdirectory(idds_server_module)
#add_subdirectory(idds_client_module)

if(UNIX)
    # Copy rtps.ini to the bin directory
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/bin/rtps.ini
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/rtps.ini
                ${CMAKE_BINARY_DIR}/bin/
        DEPENDS ${CMAKE_SOURCE_DIR}/rtps.ini
    )

    add_custom_target(copy_rtps_ini ALL DEPENDS ${CMAKE_BINARY_DIR}/bin/rtps.ini)

    # Generate setenv.sh script
    # Paths for ACE_ROOT and OPENDDS_ROOT
    set(ACE_ROOT_PATH "${CMAKE_BINARY_DIR}/_deps/opendds-build/lib")
    set(OPENDDS_ROOT_PATH "${CMAKE_BINARY_DIR}/_deps/opendds-build/ace_tao/lib")

    # Script file path
    set(ENV_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/setenv.sh")

    # Generate the setenv.sh script
    configure_file(
        ${CMAKE_SOURCE_DIR}/setenv.tmpl
        ${ENV_SCRIPT_PATH}
        @ONLY
    )

    add_custom_command(
        OUTPUT ${ENV_SCRIPT_PATH}
        COMMAND ${CMAKE_COMMAND} -E touch ${ENV_SCRIPT_PATH}
        COMMAND chmod +x ${ENV_SCRIPT_PATH}
        COMMENT "Generating environment setup script"
    )

    add_custom_target(generate_env_script ALL DEPENDS ${ENV_SCRIPT_PATH})
endif (UNIX)