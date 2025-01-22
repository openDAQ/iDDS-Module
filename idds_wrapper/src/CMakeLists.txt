project(idds_wrapper
        VERSION 1.0.0
        LANGUAGES CXX)

set(LIB_NAME idds_wrapper)

find_package(OpenDDS REQUIRED)
include(opendds_testing)

# IDL TypeSupport Library
add_library(opendaq_idl SHARED)
opendds_target_sources(opendaq_idl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../idl/iDDS.idl")
target_link_libraries(opendaq_idl PUBLIC OpenDDS::Dcps)

set(opendds_libs
  OpenDDS::Dcps # Core OpenDDS Library
  OpenDDS::InfoRepoDiscovery OpenDDS::Tcp # For run_test.pl
  OpenDDS::Rtps OpenDDS::Rtps_Udp # For run_test.pl --rtps
  opendaq_idl
)

set(SRC_Cpp iDDSDevice.cpp
            iDDSDeviceBuilder.cpp
            DataReaderListenerImpl.cpp
            CommandListenerImpl.cpp
)

set(SRC_PublicHeaders iDDSDevice.h
                      iDDSDeviceBuilder.h
                      DataReaderListenerImpl.h
                      CommandListenerImpl.h
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

target_include_directories(idds_wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(idds_wrapper PUBLIC ${opendds_libs})

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
