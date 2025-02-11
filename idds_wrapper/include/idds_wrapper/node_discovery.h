#pragma once
#include <thread>
#include <iostream>

#include "dds/dds.hpp"
#include "iDDS.hpp"

#include "idds_wrapper/idds_device_info.h"

using namespace org::eclipse::cyclonedds;

class NodeDiscovery
{
public:
    NodeDiscovery(dds::domain::DomainParticipant& participant,
                  dds::sub::DataReader<AboutNode>& reader,
                  const idds_device_info& device_info);

    ~NodeDiscovery();

    // Start the node discovery thread
    void BeginDiscovery();

    // Get Available iDDS devices
    std::vector<AboutNode> GetAvailableIDDSDevices();

    void Start();
    void Stop();

private:
    std::thread m_nodeDiscoveryThread;
    bool m_bRunning;
    dds::domain::DomainParticipant& m_participant;

    idds_device_info m_device_info;

    //AboutNode Topic
    dds::sub::DataReader<AboutNode> m_reader;

    std::vector<AboutNode> m_veciDDSNodes;
};