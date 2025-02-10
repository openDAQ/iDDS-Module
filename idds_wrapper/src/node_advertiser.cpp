#include <idds_wrapper/node_advertiser.h>

NodeAdvertiser::NodeAdvertiser()
{
}

NodeAdvertiser::~NodeAdvertiser()
{
}

void NodeAdvertiser::Start()
{
    m_nodeAdvertiserThread = std::thread(&NodeAdvertiser::BeginAdvertise, this);
}

void NodeAdvertiser::Stop()
{
    if (m_nodeAdvertiserThread.joinable())
    {
        m_nodeAdvertiserThread.join();
    }
}

/// Start the node advertiser thread
void NodeAdvertiser::BeginAdvertise()
{
}