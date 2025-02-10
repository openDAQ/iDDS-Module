#include "gtest/gtest.h"
#include "utils.h"
#include "idds_xml_params_encode.h"
#include "idds_xml_params_decode.h"
#include "idds_xml_response.h"

using namespace testing;

TEST(idds_xml_response, valid_code_only_response)
{
    idds_xml_response parser = idds_xml_response();
    parser.add_code(0);
    auto[result, message] = parser.build();
    EXPECT_EQ(result, idds_xml_error::ok);
    EXPECT_EQ(message, "<methodResponse><code>0</code><message /><params /></methodResponse>");
}

TEST(idds_xml_response, invalid_response_no_code)
{
    idds_xml_response parser = idds_xml_response();
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::response_code_not_set, result);
    EXPECT_EQ("", message);
}

TEST(idds_xml_response, invalid_response_code_range)
{
    idds_xml_response parser = idds_xml_response();

    parser.add_code(256);
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::response_code_range_invalid, result);
    EXPECT_EQ("", message);

    parser.add_code(-2);
    auto[resultNeg, messageNeg] = parser.build();

    EXPECT_EQ(idds_xml_error::response_code_range_invalid, resultNeg);
    EXPECT_EQ("", messageNeg);
}

TEST(idds_xml_response, valid_simple_response_with_message)
{
    idds_xml_response parser = idds_xml_response();
    parser.add_code(0);
    parser.add_opt_message("lorem ipsum");
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ("<methodResponse><code>0</code><message>lorem ipsum</message><params /></methodResponse>", message);
}

TEST(idds_xml_response, valid_simple_response_with_params_single)
{
    idds_xml_response parser = idds_xml_response();
    parser.add_code(0);
    parser.add_param(idds_xml_params_encode<int>("TestParamName", 16).get_parsed_string());
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ("<methodResponse><code>0</code><message /><params><param><name>TestParamName</name><value><Int32>16</Int32></value></param></params></methodResponse>", message);
}

TEST(idds_xml_response, valid_simple_response_with_params_vector)
{
    idds_xml_response parser = idds_xml_response();
    parser.add_code(0);
    parser.add_param(idds_xml_params_encode<std::vector<std::string>>("TestParamName", {"Hello", "World"}).get_parsed_string());
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ("<methodResponse><code>0</code><message /><params><param><name>TestParamName</name><value><ListOfString><String>Hello</String><String>World</String></ListOfString></value></param></params></methodResponse>", message);
}

TEST(idds_xml_response, valid_simple_response_with_two_params)
{
    idds_xml_response parser = idds_xml_response();
    parser.add_code(0);
    parser.add_param(idds_xml_params_encode<std::string>("TestParamHello", "Hello").get_parsed_string());
    parser.add_param(idds_xml_params_encode<std::string>("TestParamWorld", "World").get_parsed_string());
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ("<methodResponse><code>0</code><message /><params><param><name>TestParamHello</name><value><String>Hello</String></value></param><param><name>TestParamWorld</name><value><String>World</String></value></param></params></methodResponse>", message);
}

TEST(idds_xml_response, invalid_xml_response)
{
    idds_xml_response parser = idds_xml_response("I am an invalid xml");
    EXPECT_EQ(idds_xml_error::invalid_xml, parser.parse());
}

TEST(idds_xml_response, invalid_xml_response_no_code)
{
    idds_xml_response parser = idds_xml_response("<methodResponse></methodResponse>");
    EXPECT_EQ(idds_xml_error::not_schema_compliant_mandatory_missing, parser.parse());
}

TEST(idds_xml_response, invalid_xml_response_no_code_set)
{
    idds_xml_response parser = idds_xml_response("<methodResponse><code/></methodResponse>");
    EXPECT_EQ(idds_xml_error::not_schema_compliant_invalid_value, parser.parse());
}

TEST(idds_xml_response, invalid_xml_response_code_set)
{
    idds_xml_response parser = idds_xml_response("<methodResponse><code>2</code></methodResponse>");
    EXPECT_EQ(idds_xml_error::ok, parser.parse());
    EXPECT_EQ(2, parser.get_code());
}

TEST(idds_xml_response, invalid_xml_response_message)
{
    idds_xml_response parser = idds_xml_response("<methodResponse><message>Hello World</message><code>2</code></methodResponse>");
    EXPECT_EQ(idds_xml_error::ok, parser.parse());
    EXPECT_EQ("Hello World", parser.get_message());
}

TEST(idds_xml_response, invalid_xml_response_params)
{
    idds_xml_response parser = idds_xml_response("<methodResponse><code>0</code><message /><params><param><name>Pi</name><value><Float>3.14</Float></value></param><param><name>Hello</name><value><String>World</String></value></param></params></methodResponse>");
    EXPECT_EQ(idds_xml_error::ok, parser.parse());
    EXPECT_EQ(parser.get_params().size(), 2);

    idds_xml_params_decode<float> floatDecode(parser.get_params()[0]);
    auto [floatNameErr, floatName] = floatDecode.get_name();
    EXPECT_EQ(floatNameErr, idds_xml_error::ok);
    EXPECT_EQ(floatName, "Pi");
    auto [floatValueErr, floatValue] = floatDecode.get_value();
    EXPECT_EQ(floatValueErr, idds_xml_error::ok);
    EXPECT_FLOAT_EQ(floatValue, 3.14);

    idds_xml_params_decode<std::string> stringDecode(parser.get_params()[1]);
    auto [nameErr, name] = stringDecode.get_name();
    EXPECT_EQ(nameErr, idds_xml_error::ok);
    EXPECT_EQ(name, "Hello");
    auto [valueErr, value] = stringDecode.get_value();
    EXPECT_EQ(valueErr, idds_xml_error::ok);
    EXPECT_EQ(value, "World");

}

TEST(idds_xml_response, invalid_xml_response_parse_and_build)
{
    std::string testString = "<methodResponse><code>0</code><message /><params><param><name>Pi</name><value><Float>3.14</Float></value></param><param><name>Hello</name><value><String>World</String></value></param></params></methodResponse>";
    idds_xml_response parser = idds_xml_response(testString);

    EXPECT_EQ(idds_xml_error::ok, parser.parse());

    auto[result, message] = parser.build();
    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ(strip_whitespace(testString), strip_whitespace(message));
}
