#include <idds_wrapper/iDDSDeviceBuilder.h>
#include <idds_wrapper/iDDSDevice.h>
#include <idds_xml_request.h>
#include <idds_xml_params_decode.h>
#include <idds_xml_params_encode.h>

#include <iostream>

int main()
{
    std::string xml_message = "<methodCall><methodName>General.StartOperating</methodName><params /></methodCall>";

    idds_xml_request parser(xml_message);
    auto err = parser.parse();

    if (parser.parse() == idds_xml_error::ok)
    {
        std::cout << "Method name: " << parser.get_method_name() << std::endl;
        std::cout << "Params size: " << parser.get_params().size() << std::endl;
    }
    else
    {
        std::cout << "Error parsing xml" << std::endl;
    }

    return 0;
}
