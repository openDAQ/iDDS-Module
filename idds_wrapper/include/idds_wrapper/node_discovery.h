#pragma once
#include <thread>
#include <iostream>

#include "dds/dds.hpp"
#include "iDDS.hpp"

#include "idds_wrapper/idds_common.h"

using namespace org::eclipse::cyclonedds;

class NodeDiscovery
{
public:
    NodeDiscovery(dds::domain::DomainParticipant& participant,
                  dds::sub::DataReader<AboutNode>& reader,
                  const IddsDeviceInfo& device_info);

    ~NodeDiscovery();

    // Start the node discovery thread
    void beginDiscovery();

    // Get Available iDDS devices
    std::vector<AboutNode> getAvailableIDDSDevices();

    void start();
    void stop();

private:
    std::thread m_nodeDiscoveryThread;
    bool m_bRunning;
    dds::domain::DomainParticipant& m_participant;

    IddsDeviceInfo m_device_info;

    //AboutNode Topic
    dds::sub::DataReader<AboutNode> m_reader;

    std::vector<AboutNode> m_veciDDSNodes;
};
