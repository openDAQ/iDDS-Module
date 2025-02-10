project(idds_xml_parser
        VERSION 1.0.0
        LANGUAGES CXX)

set(LIB_NAME ${PROJECT_NAME})

set(SRC_Cpp idds_xml_params_decode.cpp
            idds_xml_params_encode.cpp
            idds_xml_request.cpp
            idds_xml_response.cpp
)

set(SRC_PublicHeaders ../include/idds_xml_error.h
        	      ../include/idds_xml_params_decode.h
        	      ../include/idds_xml_params_encode.h
        	      ../include/idds_xml_request.h
        	      ../include/idds_xml_response.h
        	      ../include/idds_xml_types.h
        	      ../include/idds_xml_utils.h
)

set(SRC_PrivateHeaders
)

#end_include(${TARGET_FOLDER_NAME} SRC_PrivateHeaders)
#end_include(${TARGET_FOLDER_NAME} SRC_PublicHeaders)

add_library(${PROJECT_NAME} SHARED
    ${SRC_Cpp}
    ${SRC_PublicHeaders}
    ${SRC_PrivateHeaders}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}      PUBLIC pugixml)

add_dependencies(idds_xml_parser pugixml)

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