#ifndef IDDSDEVICE_H
#define IDDSDEVICE_H

#include "MessengerTypeSupportImpl.h"
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
    using iDDSNodeUniqueID = std::string;

    /// Constructor
    explicit iDDSDevice(const iDDSNodeUniqueID node_id);

    /// Destructor
    ~iDDSDevice();

    /// Starts the iDDS server spawning the advertiser and listener threads
    void StartServer();

    /// Returns a vector of available iDDS devices
    std::vector<iDDSNodeUniqueID> GetAvailableIDDSDevices();

    /// Send iDDS message to a specific nodeID
    int SendIDDSMessage(const iDDSNodeUniqueID destination_node_id, const std::string message_);

    /// Print received iDDS messages
    void PrintReceivedIDDSMessages();

private:
    /// Advertise node thread
    void NodeAdvertiser();

    /// Listen for Node Advertisement essages
    int ListenForNodeAdvertisementMessages();

    /// Listen for Command Messages
    int ListenForCommandMessages();

    /// Method to setup OpenDDS domain participant factory
    int SetupiDDSDevice();

    /// Helper method to send advertisement message
    int SendAdvertisementMessage();

private:
    iDDSNodeUniqueID node_id;
    std::vector<std::string> received_messages;

    std::thread advertiser_thread;
    std::thread listener_thread;
    std::thread command_listener_thread;
    bool m_bRunning;

    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;

    DataReaderListenerImpl* listenerNodeAdvertisement_impl;
    DDS::DataReaderListener_var listenerNodeAdvertisement;

    CommandListenerImpl* listenerCommand_impl;
    DDS::DataReaderListener_var listenerCommand;

};

#endif // IDDSDEVICE_H
