#include <idds_wrapper/message_listener.h>

MessageListener::MessageListener()
{
}

MessageListener::~MessageListener()
{
}

void MessageListener::Start()
{
    m_messageListenerThread = std::thread(&MessageListener::BeginMessageParser, this);
}

void MessageListener::Stop()
{
    if (m_messageListenerThread.joinable())
    {
        m_messageListenerThread.join();
    }
}


/// Start the message listener thread
void MessageListener::BeginMessageParser()
{
}