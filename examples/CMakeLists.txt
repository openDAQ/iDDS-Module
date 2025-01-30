cmake_minimum_required(VERSION 3.3...3.27)

project(idds_wrapper_examples LANGUAGES CXX)

add_executable(App1
  App1.cpp
)
add_dependencies(App1 idds_wrapper)
target_link_libraries(App1 PRIVATE idds_wrapper)

add_executable(App2
  App2.cpp
)
add_dependencies(App2 idds_wrapper)
target_link_libraries(App2 PRIVATE idds_wrapper)

add_executable(App3
  App3.cpp
)
add_dependencies(App3 idds_wrapper)
target_link_libraries(App3 PRIVATE idds_wrapper)

add_executable(idds_server_module_example
  idds_server.cpp
)
add_dependencies(idds_server_module_example idds_wrapper daq::opendaq)
target_link_libraries(idds_server_module_example PUBLIC daq::opendaq
                                      PUBLIC idds_wrapper

)

add_executable(idds_client_module_example
  idds_client.cpp
)
add_dependencies(idds_client_module_example idds_wrapper daq::opendaq)
target_link_libraries(idds_client_module_example PUBLIC daq::opendaq
                                      PUBLIC idds_wrapper

)