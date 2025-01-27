#ifndef IDDSDEVICE_H
#define IDDSDEVICE_H

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

#include "dds/dds.hpp"

/* Include data type and specific traits to be used with the C++ DDS API. */
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

    /// Returns a vector of available iDDS devices
    std::vector<std::string> GetAvailableIDDSDevices();

    /// Send iDDS message to a specific nodeID
    int SendIDDSMessage(const std::string destination_node_id, const std::string message_);

    /// Print received iDDS messages
    void PrintReceivedIDDSMessages();

private:
    /// Advertise node thread
    void NodeAdvertiser();

    /// Node Discovery thread
    void NodeDiscovery();

    /// Message Listener thread
    void MessageListener();

    /// Helper method to send advertisement message
    int SendAdvertisementMessage();

private:
    std::string node_id;
    std::string manufacturer;
    std::string productType;
    std::string serialNumber;
    std::string hwVersion;
    std::string swVersion;
    std::string ipAddress;
    std::vector<std::string> received_messages;

    std::thread advertiser_thread;
    std::thread node_discovery_thread;
    std::thread message_listener_thread;
    bool m_bRunning;

    dds::domain::DomainParticipant participant;

    //AboutNode Topic
    dds::topic::Topic<iDDS::AboutNode> m_aboutNodeTopic;
    dds::sub::Subscriber m_aboutNodesubscriber;
    dds::sub::DataReader<iDDS::AboutNode> m_aboutNodeReader;
    dds::pub::Publisher m_aboutNodepublisher;
    dds::pub::DataWriter<iDDS::AboutNode> m_aboutNodewriter;

    //Message Topic
    dds::topic::Topic<iDDS::Message> m_MessageTopic;
    dds::sub::Subscriber m_MessageSubscriber;
    dds::sub::DataReader<iDDS::Message> m_MessageReader;
    dds::pub::Publisher m_MessagePublisher;
    dds::pub::DataWriter<iDDS::Message> m_MessageWriter;

    std::vector<iDDS::AboutNode> m_veciDDSNodes;
    std::vector<iDDS::Message> m_veciDDSMessages;
};

#endif // IDDSDEVICE_H
