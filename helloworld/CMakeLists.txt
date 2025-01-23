cmake_minimum_required(VERSION 3.16)
project(helloworld LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# Add the CycloneDDS-CXX and CycloneDDS core libraries installation directory to the CMake search path
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/cyclonedds-install")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/cyclonedds-cxx-install")

if(NOT TARGET CycloneDDS-CXX::ddscxx)
  find_package(CycloneDDS-CXX REQUIRED)
endif()

idlcxx_generate(TARGET helloworlddata FILES HelloWorldData.idl WARNINGS no-implicit-extensibility)

add_executable(ddscxxHelloworldPublisher publisher.cpp)
add_executable(ddscxxHelloworldSubscriber subscriber.cpp)

target_link_libraries(ddscxxHelloworldPublisher CycloneDDS-CXX::ddscxx helloworlddata)
target_link_libraries(ddscxxHelloworldSubscriber CycloneDDS-CXX::ddscxx helloworlddata)

if(DEFINED ENV{SYSTEM_TEAMFOUNDATIONSERVERURI})
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND ANALYZER STREQUAL "on")
    target_compile_options(ddscxxHelloworldPublisher PRIVATE -fno-analyzer)
    target_compile_options(ddscxxHelloworldSubscriber PRIVATE -fno-analyzer)
  endif()
endif()

set_property(TARGET ddscxxHelloworldPublisher PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET ddscxxHelloworldSubscriber PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
