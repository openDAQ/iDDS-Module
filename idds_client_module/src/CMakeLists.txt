project(idds_cl_module LANGUAGES CXX VERSION 1.0.0)

set(SRC_Cpp module_dll.cpp
            idds_client_module_impl.cpp
            idds_device_impl.cpp
)

set(SRC_PublicHeaders ../include/${TARGET_FOLDER_NAME}/common.h
                      ../include/${TARGET_FOLDER_NAME}/module_dll.h
                      ../include/${TARGET_FOLDER_NAME}/idds_client_module_impl.h
                      ../include/${TARGET_FOLDER_NAME}/idds_device_impl.h
)

set(SRC_PrivateHeaders
)

add_library(${PROJECT_NAME} SHARED
    ${SRC_Cpp}
    ${SRC_PublicHeaders}
    ${SRC_PrivateHeaders}
)

# This is workaround for the issue with the generated files
set_source_files_properties ("${CMAKE_BINARY_DIR}/generated/iDDS.hpp"
                             "${CMAKE_BINARY_DIR}/generated/iDDS.cpp"
                             PROPERTIES
                             GENERATED TRUE
)

target_link_libraries(${PROJECT_NAME} PUBLIC daq::opendaq
                                      PUBLIC idds_wrapper

)

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                                               $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
                                               $<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SRC_PublicHeaders}")

opendaq_set_module_properties(${PROJECT_NAME} ${PROJECT_VERSION_MAJOR})
create_version_header(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION include/${TARGET_FOLDER_NAME}
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
endif()
