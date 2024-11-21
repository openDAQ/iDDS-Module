cmake_minimum_required(VERSION 3.3...3.27)

project(idds_wrapper_examples LANGUAGES CXX)

add_executable(App1
  App1.cpp
)
target_link_libraries(App1 PRIVATE idds_wrapper)

add_executable(App2
  App2.cpp
)
target_link_libraries(App2 PRIVATE idds_wrapper)

add_executable(App3
  App3.cpp
)
target_link_libraries(App3 PRIVATE idds_wrapper)

add_executable(idds_server_module_example
  idds_server.cpp
)
target_link_libraries(idds_server_module_example PUBLIC daq::opendaq
                                      PUBLIC idds_wrapper

)