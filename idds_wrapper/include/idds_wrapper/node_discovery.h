#pragma once

#include <thread>
#include <iostream>

class NodeDiscovery
{
public:
    NodeDiscovery();
    ~NodeDiscovery();

    // Start the node discovery thread
    void BeginDiscovery();

    void Start();
    void Stop();

private:
    std::thread m_nodeDiscoveryThread;
};