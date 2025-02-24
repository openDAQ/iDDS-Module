set(TEST_APP test_idds_xml-parser)

set(TEST_SOURCES test_app.cpp
                 test_idds_xml_request.cpp
                 test_idds_xml_response.cpp
                 test_idds_xml_params_enconding.cpp
                 test_idds_xml_params_decoding.cpp
)

if (MSVC)
    add_compile_options(/bigobj)
endif()

add_executable(${TEST_APP} ${TEST_SOURCES}
)

target_link_libraries(${TEST_APP} PRIVATE daq::test_utils
                                  PUBLIC pugixml
                                  PUBLIC idds_xml_parser
)

add_dependencies(${TEST_APP} idds_xml_parser
)

add_test(NAME ${TEST_APP}
         COMMAND $<TARGET_FILE_NAME:${TEST_APP}>
         WORKING_DIRECTORY bin
)

set_target_properties(${TEST_APP} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_APP}>)

if (OPENDAQ_ENABLE_COVERAGE)
    setup_target_for_coverage(${TEST_APP}coverage ${TEST_APP} ${TEST_APP}coverage)
endif()