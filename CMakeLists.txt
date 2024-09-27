cmake_minimum_required(VERSION 3.25)
set(REPO_NAME idds_module)
set(REPO_OPTION_PREFIX idds)

project(${REPO_NAME} VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MESSAGE_CONTEXT ${REPO_NAME})
set(CMAKE_MESSAGE_CONTEXT_SHOW ON CACHE BOOL "Show CMake message context")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CommonUtils)
setup_repo(${REPO_OPTION_PREFIX})

add_subdirectory(external)
add_subdirectory(idds_wrapper)
add_subdirectory(idds_server_module)
#add_subdirectory(idds_client_module)
add_subdirectory(examples)
