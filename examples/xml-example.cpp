#include <idds_wrapper/idds_device_builder.h>
#include <idds_wrapper/idds_device_impl.h>
#include <idds_xml_request.h>
#include <idds_xml_params_decode.h>
#include <idds_xml_params_encode.h>

#include <iostream>

int main()
{
    std::string xml_StartOperating = "<methodCall><methodName>General.StartOperating</methodName><params /></methodCall>";
    std::string xml_GetAttribute = "<methodCall> <methodName>Configuration.GetAttribute</methodName> <params> <param> <name>GetValueOf</name> <value><ListOfString> <String>General/Id/LogicalNodeId</String> <String>General/WatchDog</String> </ListOfString></value> </param> </params></methodCall>";

    idds_xml_request parser(xml_GetAttribute);
    auto err = parser.parse();

    if (parser.parse() == idds_xml_error::ok)
    {
        std::cout << "Method name: " << parser.get_method_name() << std::endl;
        std::cout << "Params size: " << parser.get_params().size() << std::endl;

        for (auto param : parser.get_params())
        {
            std::cout << "Param name: " << param << std::endl;
        }
    }
    else
    {
        std::cout << "Error parsing xml" << std::endl;
    }

    return 0;
}
