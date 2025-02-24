#pragma once

#include <idds_wrapper/idds_common.h>

#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

using ParamList = std::vector<std::string>;
using Callback = std::function<void(const ParamList&, std::string&)>;

class CommandProcessor
{
public:
    CommandProcessor();
    ~CommandProcessor();

    // Register a callback for a given method
    void registerCallback(const std::string& method, Callback&& callback);

    // Populate the map with commands
    void populateCommandsXML();

    // Invoke callback
    IddsWrapperErrCode processCommand(const std::string& method, const ParamList& params, std::string& response);

    // Get supported commands
    std::vector<std::string> getSupportedCommands();

    // Get command XML
    std::string getCommandXML(const std::string& command);

    private:
        std::unordered_map<std::string, Callback> m_mapCommandCallbacks;
        std::unordered_map<std::string, std::string> m_mapCommandXML;
};
