#include <idds_wrapper/node_discovery.h>

NodeDiscovery::NodeDiscovery()
{
}

NodeDiscovery::~NodeDiscovery()
{
}

void NodeDiscovery::Start()
{
    m_nodeDiscoveryThread = std::thread(&NodeDiscovery::BeginDiscovery, this);
}

void NodeDiscovery::Stop()
{
    if (m_nodeDiscoveryThread.joinable())
    {
        m_nodeDiscoveryThread.join();
    }
}

/// Start the node discovery thread
void NodeDiscovery::BeginDiscovery()
{
}