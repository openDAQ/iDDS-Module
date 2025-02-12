#pragma once

#include <thread>
#include <iostream>

#include "dds/dds.hpp"
#include "iDDS.hpp"
#include "idds_wrapper/idds_common.h"

using namespace org::eclipse::cyclonedds;

class NodeAdvertiser
{
public:
    NodeAdvertiser(dds::pub::DataWriter<AboutNode>& writer, const idds_device_info& device_info);
    ~NodeAdvertiser();

    // Start the node advertiser thread
    void beginAdvertise();

    void start();
    void stop();

private:
    std::thread m_nodeAdvertiserThread;
    bool m_bRunning;

    idds_device_info m_device_info;

    //AboutNode Topic
    dds::pub::DataWriter<AboutNode> m_writer;

    /// Helper method to send advertisement message
    int sendAdvertisementMessage();
};