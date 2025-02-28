#pragma once
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include <idds_wrapper/idds_common.h>
#include <idds_wrapper/command_processor.h>
#include <idds_wrapper/channel_streamer.h>
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
                   const IddsDeviceInfo& device_info,
                   ChannelStreamer& channelStreamer,
                   const QoSConfig& messageReaderQoS);

    ~CommandHandler();

    // Start the message listener thread
    void BeginMessageParser();

    void start();
    void stop();

    /// Send iDDS message to a specific nodeID
    IddsWrapperErrCode sendIDDSMessage(const std::string destination_node_id, const std::string& message);

    /// Get Received IDDSMessages
    std::vector<Message> getReceivedIDDSMessages() { return m_veciDDSMessages; }

    /// Get supported iDDS commands
    std::vector<std::string> getSupportedCommands() { return m_commandProcessor.getSupportedCommands(); }

    // Get command XML
    std::string getCommandXML(const std::string& command) { return m_commandProcessor.getCommandXML(command); }

    /// Publishes a command and waits for a reply or timeout
    IddsWrapperErrCode publishCommandAndWaitForReply(const std::string& destination_node_id, const std::string& message);

private:
    /// Parse incoming idds messages
    IddsWrapperErrCode parseMessage(const Message& msg, std::string& response);

    /// Reply was received
    void setReplyAvailable();

    /// Register callbacks in the command processor
    void registerCallbacks();

    /// Prepare reply message
    void prepareReply(std::string& reply, const IddsReturnCode returnCode);

    /// Prepare reply message with params
    void prepareReply(std::string& reply, const IddsReturnCode returnCode, std::vector<std::string> params);

    /// Translate idds-return_code to IddsWrapperErrCode
    IddsReturnCode translateReturnCode(const IddsWrapperErrCode returnCode);

    /// Prepare XML response to be sent back after GetAttribute command
    std::string prepareXMLResponse(const std::string& value);

    // Helper method to extract channel name and ID from the reply
    void extractChannelNameAndID(const std::string& input);

private:
    std::thread m_commandHandlerThread;
    bool m_bRunning;
    bool  m_bReplyAvailable = false;
    std::string m_strReply;

    std::mutex mtx;
    std::condition_variable cv;

    IddsDeviceInfo m_device_info;

    dds::domain::DomainParticipant& m_participant;
    //Message Topic
    dds::topic::Topic<Message>      m_MessageTopic;
    dds::sub::Subscriber            m_MessageSubscriber;
    dds::sub::DataReader<Message>   m_MessageReader;
    dds::pub::Publisher             m_MessagePublisher;
    dds::pub::DataWriter<Message>   m_MessageWriter;

    CommandProcessor m_commandProcessor;
    ChannelStreamer& m_channelStreamer;

    std::vector<Message> m_veciDDSMessages;
};
