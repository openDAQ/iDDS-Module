#include "gtest/gtest.h"
#include <cstdint>
#include "idds_xml_params_encode.h"

using namespace testing;

TEST(idds_xml_parser_param, enconding_valid_param_int8)
{
    idds_xml_params_encode<int8_t> idds_param = idds_xml_params_encode<int8_t>("TestParamName", 16);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><Byte>16</Byte></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_int8)
{
    idds_xml_params_encode<std::vector<int8_t>> idds_param = idds_xml_params_encode<std::vector<int8_t>>("TestParamName", std::vector<int8_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfByte><Byte>16</Byte><Byte>32</Byte><Byte>64</Byte></ListOfByte></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_uint8)
{
    idds_xml_params_encode<uint8_t> idds_param = idds_xml_params_encode<uint8_t>("TestParamName", 16);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><SByte>16</SByte></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_uint8)
{
    idds_xml_params_encode<std::vector<uint8_t>> idds_param = idds_xml_params_encode<std::vector<uint8_t>>("TestParamName", std::vector<uint8_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfSByte><SByte>16</SByte><SByte>32</SByte><SByte>64</SByte></ListOfSByte></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_int16)
{
    idds_xml_params_encode<int16_t> idds_param = idds_xml_params_encode<int16_t>("TestParamName", 16);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><Int16>16</Int16></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_int16)
{
    idds_xml_params_encode<std::vector<int16_t>> idds_param = idds_xml_params_encode<std::vector<int16_t>>("TestParamName", std::vector<int16_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfInt16><Int16>16</Int16><Int16>32</Int16><Int16>64</Int16></ListOfInt16></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_uint16)
{
    idds_xml_params_encode<uint16_t> idds_param = idds_xml_params_encode<uint16_t>("TestParamName", 16);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><UInt16>16</UInt16></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_uint16)
{
    idds_xml_params_encode<std::vector<uint16_t>> idds_param = idds_xml_params_encode<std::vector<uint16_t>>("TestParamName", std::vector<uint16_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfUInt16><UInt16>16</UInt16><UInt16>32</UInt16><UInt16>64</UInt16></ListOfUInt16></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_int32)
{
    idds_xml_params_encode<int32_t> idds_param = idds_xml_params_encode<int32_t>("TestParamName", 32);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><Int32>32</Int32></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_int32)
{
    idds_xml_params_encode<std::vector<int32_t>> idds_param = idds_xml_params_encode<std::vector<int32_t>>("TestParamName", std::vector<int32_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfInt32><Int32>16</Int32><Int32>32</Int32><Int32>64</Int32></ListOfInt32></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_uint32)
{
    idds_xml_params_encode<uint32_t> idds_param = idds_xml_params_encode<uint32_t>("TestParamName", 32);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><UInt32>32</UInt32></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_uint32)
{
    idds_xml_params_encode<std::vector<uint32_t>> idds_param = idds_xml_params_encode<std::vector<uint32_t>>("TestParamName", std::vector<uint32_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfUInt32><UInt32>16</UInt32><UInt32>32</UInt32><UInt32>64</UInt32></ListOfUInt32></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_int64)
{
    idds_xml_params_encode<int64_t> idds_param = idds_xml_params_encode<int64_t>("TestParamName", 32);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><Int64>32</Int64></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_int64)
{
    idds_xml_params_encode<std::vector<int64_t>> idds_param = idds_xml_params_encode<std::vector<int64_t>>("TestParamName", std::vector<int64_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfInt64><Int64>16</Int64><Int64>32</Int64><Int64>64</Int64></ListOfInt64></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_uint64)
{
    idds_xml_params_encode<uint64_t> idds_param = idds_xml_params_encode<uint64_t>("TestParamName", 32);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><UInt64>32</UInt64></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_uint64)
{
    idds_xml_params_encode<std::vector<uint64_t>> idds_param = idds_xml_params_encode<std::vector<uint64_t>>("TestParamName", std::vector<uint64_t>({16, 32, 64}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfUInt64><UInt64>16</UInt64><UInt64>32</UInt64><UInt64>64</UInt64></ListOfUInt64></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_bool_true)
{
    idds_xml_params_encode<bool> idds_param = idds_xml_params_encode<bool>("TestParamName", true);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><Boolean>true</Boolean></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_bool)
{
    idds_xml_params_encode<std::vector<bool>> idds_param = idds_xml_params_encode<std::vector<bool>>("TestParamName", std::vector<bool>({true, false, true}));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfBoolean><Boolean>true</Boolean><Boolean>false</Boolean><Boolean>true</Boolean></ListOfBoolean></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_bool_false)
{
    idds_xml_params_encode<bool> idds_param = idds_xml_params_encode<bool>("TestParamName", false);
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><Boolean>false</Boolean></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_string_true)
{
    idds_xml_params_encode<std::string> idds_param = idds_xml_params_encode<std::string>("TestParamName", std::string("foo"));
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><String>foo</String></value></param>");
}

TEST(idds_xml_parser_param, enconding_valid_param_list_string)
{
    idds_xml_params_encode<std::vector<std::string>> idds_param = idds_xml_params_encode<std::vector<std::string>>("TestParamName", {"foo", "bar"});
    std::string strParam = idds_param.get_parsed_string();

    EXPECT_EQ(strParam, "<param><name>TestParamName</name><value><ListOfString><String>foo</String><String>bar</String></ListOfString></value></param>");
}

