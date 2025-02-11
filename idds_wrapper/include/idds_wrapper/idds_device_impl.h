#pragma once
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

#include "idds_wrapper/idds_device_info.h"
#include <idds_wrapper/node_advertiser.h>
#include <idds_wrapper/node_discovery.h>
#include <idds_wrapper/command_handler.h>
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
    void StartServer();

    /// Stop the iDDS server
    void StopServer();

    /// Returns a vector of available iDDS devices
    std::vector<std::string> GetAvailableIDDSDevices();

    /// Send iDDS message to a specific nodeID
    int SendIDDSMessage(const std::string destination_node_id, const std::string message_);

    /// Print received iDDS messages
    void PrintReceivedIDDSMessages();

private:
    idds_device_info m_idds_device_info;
    std::vector<std::string> received_messages;

    // DDS
    dds::domain::DomainParticipant  participant;
    dds::topic::Topic<AboutNode>    m_aboutNodeTopic;
    dds::sub::Subscriber            m_aboutNodesubscriber;
    dds::sub::DataReader<AboutNode> m_aboutNodeReader;
    dds::pub::Publisher             m_aboutNodepublisher;
    dds::pub::DataWriter<AboutNode> m_aboutNodewriter;

    NodeAdvertiser  m_nodeAdvertiser;
    NodeDiscovery   m_nodeDiscovery;
    CommandHandler  m_commandHandler;

    bool m_bRunning;
};
