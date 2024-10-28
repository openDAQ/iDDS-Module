project(idds_wrapper
        VERSION 1.0.0
        LANGUAGES CXX)

set(LIB_NAME idds_wrapper)

find_package(OpenDDS REQUIRED)
include(opendds_testing)

# IDL TypeSupport Library
add_library(opendaq_idl)
opendds_target_sources(opendaq_idl PUBLIC "Messenger.idl")
target_link_libraries(opendaq_idl PUBLIC OpenDDS::Dcps)

set(opendds_libs
  OpenDDS::Dcps # Core OpenDDS Library
  OpenDDS::InfoRepoDiscovery OpenDDS::Tcp # For run_test.pl
  OpenDDS::Rtps OpenDDS::Rtps_Udp # For run_test.pl --rtps
  opendaq_idl
)

# iDDSWrapper Library
add_library(idds_wrapper
  iDDSDevice.cpp
  DataReaderListenerImpl.cpp
  CommandListenerImpl.cpp
)
target_include_directories(idds_wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(idds_wrapper PUBLIC ${opendds_libs})

opendaq_set_module_properties(${LIB_NAME} ${PROJECT_VERSION_MAJOR})
create_version_header(${LIB_NAME})

install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION include/${TARGET_FOLDER_NAME}
)
