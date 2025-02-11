#pragma once
#include <thread>
#include <iostream>

#include <idds_wrapper/idds_common.h>
#include <idds_wrapper/command_processor.h>
#include <idds_xml_request.h>
#include <idds_xml_params_decode.h>
#include <idds_xml_params_encode.h>
#include "idds_xml_response.h"

#include "dds/dds.hpp"
#include "iDDS.hpp"

using namespace org::eclipse::cyclonedds;


class CommandHandler
{
public:
    CommandHandler(dds::domain::DomainParticipant& participant,
                   const idds_device_info& device_info);

    ~CommandHandler();

    // Start the message listener thread
    void BeginMessageParser();

    void Start();
    void Stop();

    /// Send iDDS message to a specific nodeID
    int SendIDDSMessage(const std::string destination_node_id, const std::string message_);

    /// Get Received IDDSMessages
    std::vector<Message> GetReceivedIDDSMessages() { return m_veciDDSMessages; }

private:
    /// Parse incoming idds messages
    idds_wrapper_errCode parseMessage(const Message& msg, std::string& response);

    /// Register callbacks in the command processor
    void registerCallbacks();

    /// Prepare reply message
    void prepareReply(std::string& reply, const idds_returnCode returnCode);

private:
    std::thread m_commandHandlerThread;
    bool m_bRunning;

    idds_device_info m_device_info;

    dds::domain::DomainParticipant& m_participant;
    //Message Topic
    dds::topic::Topic<Message>      m_MessageTopic;
    dds::sub::Subscriber            m_MessageSubscriber;
    dds::sub::DataReader<Message>   m_MessageReader;
    dds::pub::Publisher             m_MessagePublisher;
    dds::pub::DataWriter<Message>   m_MessageWriter;

    CommandProcessor m_commandProcessor;

    std::vector<Message> m_veciDDSMessages;
};