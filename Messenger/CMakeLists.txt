cmake_minimum_required(VERSION 3.3...3.27)
project(opendds_devguide_messenger CXX)
enable_testing()

find_package(OpenDDS REQUIRED)
include(opendds_testing)

# IDL TypeSupport Library
add_library(messenger_idl_from_messenger SHARED)
opendds_target_sources(messenger_idl_from_messenger PUBLIC "Messenger.idl")
target_link_libraries(messenger_idl_from_messenger PUBLIC OpenDDS::Dcps)

set(opendds_libs
  OpenDDS::Dcps # Core OpenDDS Library
  OpenDDS::InfoRepoDiscovery OpenDDS::Tcp # For run_test.pl
  OpenDDS::Rtps OpenDDS::Rtps_Udp # For run_test.pl --rtps
  messenger_idl_from_messenger
)

# Publisher
add_executable(publisher_from_messenger
  Publisher.cpp
)
target_link_libraries(publisher_from_messenger PRIVATE ${opendds_libs})

# Subscriber
add_executable(subscriber_from_messenger
  Subscriber.cpp
  DataReaderListenerImpl.cpp
)
target_link_libraries(subscriber_from_messenger PRIVATE ${opendds_libs})
