#pragma once
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "idds_wrapper/idds_common.h"
#include <idds_wrapper/node_advertiser.h>
#include <idds_wrapper/node_discovery.h>
#include <idds_wrapper/command_handler.h>
#include <idds_wrapper/channel_streamer.h>
#include "dds/dds.hpp"
#include "iDDS.hpp"

using namespace org::eclipse::cyclonedds;

class iDDSDevice
{
public:
    /// Constructor
    explicit iDDSDevice(const std::string node_id, const std::string manufacturer = "openDAQ", const std::string productType = "model",
                        const std::string serialNumber = "serial_number", const std::string hwVersion = "", const std::string swVersion = "",
                        const std::string ipAddress = "127.0.0.1");

    /// Destructor
    ~iDDSDevice();

    /// Starts the iDDS server spawning the advertiser and listener threads
    void startServer();

    /// Stop the iDDS server
    void stopServer();

    /// Returns a vector of available iDDS devices
    std::vector<std::string> getAvailableIDDSDevices();

    /// Send iDDS message to a specific nodeID
    idds_wrapper_errCode sendIDDSMessage(const std::string destination_node_id, const std::string message_);

    /// Print received iDDS messages
    void printReceivedIDDSMessages();

    /// Subscribe to a channel for streaming data
    void subscribeToChannel(const std::string channelName);

    /// Subscribe to a channel for streaming data
    void subscribeToChannel(const int channelParamID);

    /// Get suppoprt idds commands
    std::vector<std::string> getSupportedIDDSCommands() { return m_commandHandler.getSupportedCommands(); }

    /// Request command
    idds_wrapper_errCode publishCommand(const std::string targetLogicalNodeID, const std::string command);

    /// Get Available Signal Name and ID
    std::map<std::string, ParameterID> getDiscoverableSignalNameAndID();

private:
    idds_device_info m_idds_device_info;
    std::vector<std::string> received_messages;

    // DDS
    dds::domain::DomainParticipant  m_participant;
    dds::domain::DomainParticipant  m_streamParticipant;
    dds::topic::Topic<AboutNode>    m_aboutNodeTopic;
    dds::sub::Subscriber            m_aboutNodesubscriber;
    dds::sub::DataReader<AboutNode> m_aboutNodeReader;
    dds::pub::Publisher             m_aboutNodepublisher;
    dds::pub::DataWriter<AboutNode> m_aboutNodewriter;
    dds::sub::Subscriber            m_ParameterDataSeriesSubscriber;
    dds::pub::Publisher             m_ParameterDataSeriesPublisher;

    NodeAdvertiser  m_nodeAdvertiser;
    NodeDiscovery   m_nodeDiscovery;
    CommandHandler  m_commandHandler;
    ChannelStreamer m_channelStreamer;

    bool m_bRunning;
};
