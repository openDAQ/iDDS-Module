#include <idds_wrapper/command_processor.h>

CommandProcessor::CommandProcessor()
{
    // Populate the map with commands - Proper encoding to be added in the future
    m_mapCommandXML["General.HardReset"] = "<methodCall> <methodName>General.HardReset</methodName> <params /> </methodCall>";
    m_mapCommandXML["General.StopOperating"] = "<methodCall> <methodName>General.StopOperating</methodName> <params /> </methodCall>";
    m_mapCommandXML["General.StartOperating"] = "<methodCall> <methodName>General.StartOperating</methodName> <params /> </methodCall>";
    m_mapCommandXML["Configuration.GetAttribute"] = "<methodCall> <methodName>Configuration.GetAttribute</methodName> <params> <param> <name>Attribute</name> <value><String>alistv</String></value> </param> </params> </methodCall>";
}

CommandProcessor::~CommandProcessor()
{
}

void CommandProcessor::registerCallback(const std::string& method, Callback callback)
{
    m_mapCommandCallbacks[method] = callback;
}

idds_wrapper_errCode CommandProcessor::processCommand(const std::string& method, const ParamList& params, std::string& response)
{
    if (m_mapCommandCallbacks.find(method) != m_mapCommandCallbacks.end())
    {
        m_mapCommandCallbacks[method](params, response);
        return idds_wrapper_errCode::OK;
    }
    else
    {
        std::cout << "Method not found: " << method << std::endl;
        return idds_wrapper_errCode::CALLBACK_NOT_FOUND;
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
std::string CommandProcessor::getCommandXML(std::string command)
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
