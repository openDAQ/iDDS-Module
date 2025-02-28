cmake_minimum_required(VERSION 3.3...3.27)

project(idds_wrapper_examples LANGUAGES CXX)

add_executable(idds-simple-device
  idds-simple-device.cpp
)
add_dependencies(idds-simple-device idds_wrapper)
target_link_libraries(idds-simple-device PRIVATE idds_wrapper)

add_executable(idds-complete-device
  idds-complete-device.cpp
)
add_dependencies(idds-complete-device idds_wrapper)
target_link_libraries(idds-complete-device PRIVATE idds_wrapper)

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


add_executable(xml-parser-example
  xml-example.cpp
)
add_dependencies(xml-parser-example idds_wrapper daq::opendaq idds_xml_parser)
target_link_libraries(xml-parser-example PUBLIC daq::opendaq
                                         PUBLIC idds_wrapper
                                         PUBLIC idds_xml_parser
)
