#include "gtest/gtest.h"
#include "utils.h"
#include "idds_xml_error.h"
#include "idds_xml_request.h"
#include "idds_xml_params_decode.h"
#include "idds_xml_params_encode.h"

using namespace testing;

TEST(idds_xml_request, invalid_xml)
{
    idds_xml_request parser("I am an invalid xml");
    EXPECT_EQ(parser.parse(), idds_xml_error::invalid_xml);
}

TEST(idds_xml_request, valid_xml_invalid_schema)
{
    idds_xml_request parser("<xml></xml>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_mandatory_missing);
}

TEST(idds_xml_request, valid_xml_valid_schema)
{
    idds_xml_request parser("<methodCall><methodName>General.GetStatus</methodName></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::ok);
}

TEST(idds_xml_request, valid_xml_invalid_schema_mult_occurrences)
{
    idds_xml_request parser("<methodCall><methodName><methodName>HelloWorld</methodName></methodName></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_multiple_occurrences);
}

TEST(idds_xml_request, valid_xml_invalid_schema_missing_methodName)
{
    idds_xml_request parser("<methodCall></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_mandatory_missing);
}

TEST(idds_xml_request, valid_xml_invalid_schema_missing_methodCall)
{
    idds_xml_request parser("<methodName></methodName>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_mandatory_missing);
}

TEST(idds_xml_request, valid_xml_invalid_schema_mult_occurrences_siblings)
{
    idds_xml_request parser("<methodCall></methodCall><methodName>HelloWorld</methodName>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_mandatory_missing);
}

TEST(idds_xml_request, valid_xml_valid_schema_optional_field_params)
{
    idds_xml_request parser("<methodCall><methodName>MyName</methodName><params></params></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::ok);
}

TEST(idds_xml_request, valid_xml_invalid_schema_invalid_optional_name)
{
    idds_xml_request parser("<methodCall><methodName>MyName</methodName><foo></foo></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_invalid_optional_node);
}

TEST(idds_xml_request, valid_xml_invalid_schema_missing_mandatory)
{
    idds_xml_request parser("<methodCall><foo></foo><params></params></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_mandatory_missing);
}

TEST(idds_xml_request, valid_xml_invalid_schema_too_many_optional_field)
{
    idds_xml_request parser("<methodCall><methodName>MyName</methodName><params></params><foo></foo></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_too_many_child_nodes);
}

TEST(idds_xml_request, valid_xml_invalid_schema_too_many_optional_field_2)
{
    idds_xml_request parser("<methodCall><methodName>MyName</methodName><params></params><foo></foo><foo></foo></methodCall>");
    EXPECT_EQ(parser.parse(), idds_xml_error::not_schema_compliant_too_many_child_nodes);
}

TEST(idds_xml_request, valid_xml_retrieve_method_name)
{
    idds_xml_request parser("<methodCall><methodName>General.GetStatus</methodName><params/></methodCall>");
    auto err = parser.parse();
    EXPECT_EQ(parser.get_method_name(), "General.GetStatus");
    EXPECT_EQ(parser.get_params().size(), 0);
    EXPECT_EQ(err, idds_xml_error::ok);
}

TEST(idds_xml_request, valid_xml_retrieve_params)
{
    idds_xml_request parser("<methodCall><methodName>General.GetStatus</methodName><params><param><name>Hello</name><value><String>World</String></value></param></params></methodCall>");
    auto err = parser.parse();
    EXPECT_EQ(err, idds_xml_error::ok);
    EXPECT_EQ(parser.get_method_name(), "General.GetStatus");
    EXPECT_EQ(parser.get_params().size(), 1);

    idds_xml_params_decode<std::string> paramDecode(parser.get_params()[0]);
    auto [nameErr, name] = paramDecode.get_name();
    EXPECT_EQ(nameErr, idds_xml_error::ok);
    EXPECT_EQ(name, "Hello");
    auto [valueErr, value] = paramDecode.get_value();
    EXPECT_EQ(valueErr, idds_xml_error::ok);
    EXPECT_EQ(value, "World");
}

TEST(idds_xml_request, valid_xml_retrieve_message)
{
    idds_xml_request parser("<methodCall><methodName>General.GetStatus</methodName><params/></methodCall>");
    auto err = parser.parse();
    EXPECT_EQ(parser.get_method_name(), "General.GetStatus");
    EXPECT_EQ(parser.get_params().size(), 0);
    EXPECT_EQ(err, idds_xml_error::ok);
}

TEST(idds_xml_request, valid_xml_retrieve_several_params)
{
    idds_xml_request parser("<methodCall><methodName>General.GetStatus</methodName><params><param><name>Pi</name><value><Float>3.14</Float></value></param><param><name>Hello</name><value><String>World</String></value></param></params></methodCall>");
    auto err = parser.parse();
    EXPECT_EQ(err, idds_xml_error::ok);
    EXPECT_EQ(parser.get_method_name(), "General.GetStatus");
    EXPECT_EQ(parser.get_params().size(), 2);

    idds_xml_params_decode<float> floatDecode(parser.get_params()[0]);
    auto [floatNameErr, floatName] = floatDecode.get_name();
    EXPECT_EQ(floatNameErr, idds_xml_error::ok);
    EXPECT_EQ(floatName, "Pi");
    auto [floatValueErr, floatValue] = floatDecode.get_value();
    EXPECT_EQ(floatValueErr, idds_xml_error::ok);
    EXPECT_FLOAT_EQ(floatValue, 3.14f);

    idds_xml_params_decode<std::string> stringDecode(parser.get_params()[1]);
    auto [nameErr, name] = stringDecode.get_name();
    EXPECT_EQ(nameErr, idds_xml_error::ok);
    EXPECT_EQ(name, "Hello");
    auto [valueErr, value] = stringDecode.get_value();
    EXPECT_EQ(valueErr, idds_xml_error::ok);
    EXPECT_EQ(value, "World");
}

TEST(idds_xml_request, valid_xml_build_mandatory_field)
{
    idds_xml_request parser = idds_xml_request();
    parser.add_method_name("HelloWorld");
    auto [err, message] = parser.build();
    EXPECT_EQ(err, idds_xml_error::ok);
    EXPECT_EQ(message, "<methodCall><methodName>HelloWorld</methodName><params /></methodCall>");
}

TEST(idds_xml_request, invalid_xml_build_mandatory_field)
{
    idds_xml_request parser = idds_xml_request();
    EXPECT_EQ(parser.build(), std::make_tuple(idds_xml_error::request_method_name_not_set, ""));
}

TEST(idds_xml_request, valid_simple_response_with_params_single)
{
    idds_xml_request parser = idds_xml_request();
    parser.add_method_name("HelloWorld");
    parser.add_param(idds_xml_params_encode<int>("TestParamName", 16).get_parsed_string());
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ("<methodCall><methodName>HelloWorld</methodName><params><param><name>TestParamName</name><value><Int32>16</Int32></value></param></params></methodCall>", message);
}

TEST(idds_xml_request, valid_simple_response_with_params_vector)
{
    idds_xml_request parser = idds_xml_request();
    parser.add_method_name("HelloWorld");
    parser.add_param(idds_xml_params_encode<std::vector<std::string>>("TestParamName", {"Hello", "World"}).get_parsed_string());
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ("<methodCall><methodName>HelloWorld</methodName><params><param><name>TestParamName</name><value><ListOfString><String>Hello</String><String>World</String></ListOfString></value></param></params></methodCall>", message);
}

TEST(idds_xml_request, valid_simple_response_with_two_params)
{
    idds_xml_request parser = idds_xml_request();
    parser.add_method_name("HelloWorld");
    parser.add_param(idds_xml_params_encode<std::string>("TestParamHello", "Hello").get_parsed_string());
    parser.add_param(idds_xml_params_encode<std::string>("TestParamWorld", "World").get_parsed_string());
    auto[result, message] = parser.build();

    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ("<methodCall><methodName>HelloWorld</methodName><params><param><name>TestParamHello</name><value><String>Hello</String></value></param><param><name>TestParamWorld</name><value><String>World</String></value></param></params></methodCall>", message);
}


TEST(idds_xml_request, invalid_xml_response_parse_and_build)
{
    std::string testString = "<methodCall><methodName>HelloWorld</methodName><params><param><name>Pi</name><value><Float>3.14</Float></value></param><param><name>Hello</name><value><String>World</String></value></param></params></methodCall>";
    idds_xml_request parser = idds_xml_request(testString);

    EXPECT_EQ(idds_xml_error::ok, parser.parse());

    auto[result, message] = parser.build();
    EXPECT_EQ(idds_xml_error::ok, result);
    EXPECT_EQ(strip_whitespace(testString), strip_whitespace(message));
}


TEST(idds_xml_request, command_response)
{
    std::string testString = "<methodResponse><code>0</code><message /><params /></methodResponse>";
    idds_xml_request parser = idds_xml_request(testString);

    auto err = parser.parse();
    EXPECT_EQ(err, idds_xml_error::ok);
    EXPECT_EQ(parser.get_return_code(), 0);
}
