project(idds_wrapper
        VERSION 1.0.0
        LANGUAGES CXX)

set(LIB_NAME ${PROJECT_NAME})

# Add the CycloneDDS-CXX and CycloneDDS core libraries installation directory to the CMake search path
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/cyclonedds-install")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/cyclonedds-cxx-install")

if(NOT TARGET CycloneDDS-CXX::ddscxx)
  find_package(CycloneDDS-CXX REQUIRED)
endif()

idlcxx_generate(TARGET      idl-data
                FILES       "${CMAKE_CURRENT_SOURCE_DIR}/../idl/iDDS.idl"
                WARNINGS    no-implicit-extensibility
                OUTPUT_DIR  "${CMAKE_BINARY_DIR}/generated")

set(idds_libs CycloneDDS-CXX::ddscxx
              idl-data
)

set(SRC_Cpp           idds_device_impl.cpp
                      idds_device_builder.cpp
                      node_advertiser.cpp
                      node_discovery.cpp
                      command_handler.cpp
                      command_processor.cpp
                      channel_streamer.cpp
)

set(SRC_PublicHeaders idds_common.h
                      idds_device_impl.h
                      idds_device_builder.h
                      node_advertiser.h
                      node_discovery.h
                      command_handler.h
                      command_processor.h
                      channel_streamer.h
)

set(SRC_PrivateHeaders
)

prepend_include(${TARGET_FOLDER_NAME} SRC_PrivateHeaders)
prepend_include(${TARGET_FOLDER_NAME} SRC_PublicHeaders)

add_library(${PROJECT_NAME} SHARED
    ${SRC_Cpp}
    ${SRC_PublicHeaders}
    ${SRC_PrivateHeaders}
)

set_source_files_properties ("${CMAKE_BINARY_DIR}/generated/iDDS.hpp"
                             "${CMAKE_BINARY_DIR}/generated/iDDS.cpp"
                             PROPERTIES
                             GENERATED TRUE
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${idds_libs}
                                      PUBLIC idds_xml_parser)

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
                                                  $<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SRC_PublicHeaders}")

opendaq_set_module_properties(${LIB_NAME} ${PROJECT_VERSION_MAJOR})
create_version_header(${LIB_NAME})

install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION include/${TARGET_FOLDER_NAME}
)

# Copy the CycloneDDS files to the output directory
if (EXISTS "${CMAKE_BINARY_DIR}/cyclonedds-install/bin")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/cyclonedds-install/bin" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying CycloneDDS binaries..."
    )
endif()

if (EXISTS "${CMAKE_BINARY_DIR}/cyclonedds-cxx-install/bin")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/cyclonedds-cxx-install/bin" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying CycloneDDS-CXX binaries..."
    )
endif()
