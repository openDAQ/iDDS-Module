#include <idds_wrapper/command_processor.h>
#include "idds_xml_error.h"
#include "idds_xml_request.h"
#include "idds_xml_params_decode.h"
#include "idds_xml_params_encode.h"

CommandProcessor::CommandProcessor()
{
    populateCommandsXML();
}

CommandProcessor::~CommandProcessor()
{
}

void CommandProcessor::registerCallback(const std::string& method, Callback&& callback)
{
    m_mapCommandCallbacks[method] = std::move(callback);
}

// Populate the map with commands
void CommandProcessor::populateCommandsXML()
{
    idds_xml_error err;
    std::string message;
    idds_xml_request parser = idds_xml_request();

    // General.HardReset
    parser.add_method_name("General.HardReset");
    std::tie(err, message) = parser.build();
    m_mapCommandXML["General.HardReset"] = message;

    // General.StopOperating
    parser.add_method_name("General.StopOperating");
    std::tie(err, message) = parser.build();
    m_mapCommandXML["General.StopOperating"] = message;

    // Configuration.StartOperating
    parser.add_method_name("General.StartOperating");
    std::tie(err, message) = parser.build();
    m_mapCommandXML["General.StartOperating"] = message;

    // Configuration.GetAttribute
    parser.add_method_name("Configuration.GetAttribute");
    parser.add_param(idds_xml_params_encode<std::string>("Attribute", "alistv").get_parsed_string());
    std::tie(err, message) = parser.build();
    m_mapCommandXML["Configuration.GetAttribute"] = message;
}

IddsWrapperErrCode CommandProcessor::processCommand(const std::string& method, const ParamList& params, std::string& response)
{
    if (m_mapCommandCallbacks.find(method) != m_mapCommandCallbacks.end())
    {
        m_mapCommandCallbacks[method](params, response);
        return IddsWrapperErrCode::OK;
    }
    else
    {
        std::cout << "Method not found: " << method << std::endl;
        return IddsWrapperErrCode::CALLBACK_NOT_FOUND;
    }
}

// Get supported commands
std::vector<std::string> CommandProcessor::getSupportedCommands()
{
    std::vector<std::string> supportedCommands;
    for (auto& [key, value] : m_mapCommandCallbacks)
    {
        supportedCommands.push_back(key);
    }
    return supportedCommands;
}

// Get command XML for a given command
std::string CommandProcessor::getCommandXML(const std::string& command)
{
    if (m_mapCommandXML.find(command) != m_mapCommandXML.end())
    {
        return m_mapCommandXML[command];
    }
    else
    {
        return "";
    }
}
