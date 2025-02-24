#include "gtest/gtest.h"
#include "idds_xml_params_decode.h"

using namespace testing;

TEST(idds_xml_parser_param, decoding_invalid_param_string_no_param)
{
    idds_xml_params_decode<std::string> idds_param = idds_xml_params_decode<std::string>("<name>TestParamName</name><value><string>Hello</string></value>");
    auto [result, varName] = idds_param.get_name();
    EXPECT_EQ(result, idds_xml_error::param_parent_missing);
    EXPECT_EQ(varName, "");
}

TEST(idds_xml_parser_param, decoding_invalid_param_string_no_name)
{
    idds_xml_params_decode<std::string> idds_param = idds_xml_params_decode<std::string>("<param><value><string>Hello</string></value></param>");
    auto [result, varName] = idds_param.get_name();
    EXPECT_EQ(result, idds_xml_error::param_name_keyword_missing);
    EXPECT_EQ(varName, "");
}

TEST(idds_xml_parser_param, decoding_valid_param_name)
{
    idds_xml_params_decode<std::string> idds_param = idds_xml_params_decode<std::string>("<param><name>TestParamName</name><value></value></param>");
    auto [result, varName] = idds_param.get_name();
    EXPECT_EQ(result, idds_xml_error::ok);
    EXPECT_EQ(varName, "TestParamName");
}

TEST(idds_xml_parser_param, decoding_valid_param_string)
{
    idds_xml_params_decode<std::string> idds_param = idds_xml_params_decode<std::string>("<param><name>TestParamName</name><value><String>MyValue</String></value></param>");
    auto [resultName, varName] = idds_param.get_name();
    EXPECT_EQ(resultName, idds_xml_error::ok);
    EXPECT_EQ(varName, "TestParamName");

    auto [resultValue, varValue] = idds_param.get_value();
    EXPECT_EQ(resultValue, idds_xml_error::ok);
    EXPECT_EQ(varValue, "MyValue");
}
