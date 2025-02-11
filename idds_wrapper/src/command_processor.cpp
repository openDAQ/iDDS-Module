#include <idds_wrapper/command_processor.h>

CommandProcessor::CommandProcessor()
{
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
