#pragma once

#include <thread>
#include <iostream>

class MessageListener
{
public:
    MessageListener();
    ~MessageListener();

    // Start the message listener thread
    void BeginMessageParser();

    void Start();
    void Stop();

private:
    std::thread m_messageListenerThread;
};