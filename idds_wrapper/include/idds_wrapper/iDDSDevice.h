#ifndef IDDSDEVICE_H
#define IDDSDEVICE_H

#include "iDDSTypeSupportImpl.h"
#include "iDDSC.h"
#include "DataReaderListenerImpl.h"
#include "CommandListenerImpl.h"

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/StaticIncludes.h>
#if OPENDDS_DO_MANUAL_STATIC_INCLUDES
#include <dds/DCPS/RTPS/RtpsDiscovery.h>
#include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>

#include <ace/Log_Msg.h>

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

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

    /// Method to setup OpenDDS domain participant factory
    int SetupiDDSDevice();

    /// Helper method to send advertisement message
    int SendAdvertisementMessage();

    /// Create Node Advertiser Topic
    int CreateNodeAdvertiserTopic();

    /// Create Message Topic
    int CreateMessageTopic();

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
    std::thread command_listener_thread;
    bool m_bRunning;

    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;

    DataReaderListenerImpl* listenerNodeAdvertisement_impl;
    DDS::DataReaderListener_var listenerNodeAdvertisement;

    CommandListenerImpl* listenerCommand_impl;
    DDS::DataReaderListener_var listenerCommand;

    //AboutNode Topic
    DDS::Topic_var NodeAdvertiserTopic;
    DDS::Publisher_var NodeAdvertiserPublisher;
    DDS::DataWriter_var NodeAdvertiserWriter;
    RealTimeBackbone::AboutNodeDataWriter_var AboutNode_writer;
    DDS::Subscriber_var NodeAdvertiserSubscriber;
    DDS::DataReader_var NodeAdvertiserReader;

    //Message Topic
    DDS::Topic_var MessageTopic;
    DDS::Publisher_var MessagePublisher;
    DDS::DataWriter_var MessageWriter;
    RealTimeBackbone::MessageDataWriter_var Message_writer;
    DDS::Subscriber_var MessageSubscriber;
    DDS::DataReader_var MessageReader;
};

#endif // IDDSDEVICE_H
