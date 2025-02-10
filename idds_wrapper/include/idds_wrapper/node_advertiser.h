#pragma once

#include <thread>
#include <iostream>

class NodeAdvertiser
{
public:
    NodeAdvertiser();
    ~NodeAdvertiser();

    // Start the node advertiser thread
    void BeginAdvertise();

    void Start();
    void Stop();

private:
    std::thread m_nodeAdvertiserThread;
};