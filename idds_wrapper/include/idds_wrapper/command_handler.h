#pragma once
#include <thread>
#include <iostream>

#include "idds_wrapper/idds_device_info.h"
#include "dds/dds.hpp"
#include "iDDS.hpp"

using namespace org::eclipse::cyclonedds;


class CommandHandler
{
public:
    CommandHandler(dds::domain::DomainParticipant& participant, const idds_device_info& device_info);
    ~CommandHandler();

    // Start the message listener thread
    void BeginMessageParser();

    void Start();
    void Stop();

    /// Send iDDS message to a specific nodeID
    int SendIDDSMessage(const std::string destination_node_id, const std::string message_);

    /// Get Received IDDSMessages
    std::vector<Message> GetReceivedIDDSMessages();

private:
    std::thread m_commandHandlerThread;
    bool m_bRunning;

    idds_device_info m_device_info;

    //Message Topic
    dds::topic::Topic<Message>      m_MessageTopic;
    dds::sub::Subscriber            m_MessageSubscriber;
    dds::sub::DataReader<Message>   m_MessageReader;
    dds::pub::Publisher             m_MessagePublisher;
    dds::pub::DataWriter<Message>   m_MessageWriter;

    std::vector<Message> m_veciDDSMessages;
};