#include <idds_wrapper/idds_device_impl.h>
#include <iostream>
#include <fstream>
#include <filesystem>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------

static const int c_nDomainID = 0;
static const int c_nStreamingDomainID = -1; //Set to -1 (minus one) if not currently known.
static const char node_advertiser_topic[] = "AboutNode";
static const char message_topic[] = "Message";
static const char rtps_file[] = "rtps.ini";


iDDSDevice::iDDSDevice(const std::string node_id, const std::string manufacturer, const std::string productType,
                       const std::string serialNumber, const std::string hwVersion, const std::string swVersion,
                       const std::string ipAddress)
                       : m_idds_device_info{node_id, manufacturer, productType,
                                serialNumber, hwVersion, swVersion, ipAddress}
                       // cycloneDDS specific initialization
                       , participant(c_nDomainID)
                       , m_aboutNodeTopic(participant, node_advertiser_topic)
                       , m_aboutNodesubscriber(participant)
                       , m_aboutNodeReader(m_aboutNodesubscriber, m_aboutNodeTopic)
                       , m_aboutNodepublisher(participant)
                       , m_aboutNodewriter(m_aboutNodepublisher, m_aboutNodeTopic)
                       , m_nodeAdvertiser(m_aboutNodewriter, m_idds_device_info)
                       , m_nodeDiscovery(m_aboutNodeReader, m_idds_device_info)
                       , m_commandHandler(participant, m_idds_device_info)
{
}

// Destructor
iDDSDevice::~iDDSDevice()
{
    // Join threads if they are joinable
    if (m_bRunning)
    {
        StopServer();
    }
}

// StartServer method
void iDDSDevice::StartServer()
{
    // Start the server
    m_bRunning = true;

    m_nodeAdvertiser.Start();
    m_nodeDiscovery.Start();
    m_commandHandler.Start();
}

// Stop the iDDS server
void iDDSDevice::StopServer()
{
    m_bRunning = false;

    m_nodeAdvertiser.Stop();
    m_nodeDiscovery.Stop();
    m_commandHandler.Stop();
}

// GetAvailableIDDSDevices method
std::vector<std::string> iDDSDevice::GetAvailableIDDSDevices()
{
    std::vector<std::string> available_devices;

    std::vector<AboutNode> veciDDSNodes = m_nodeDiscovery.GetAvailableIDDSDevices();

    for (auto &node : veciDDSNodes)
    {
        available_devices.push_back(node.logicalNodeID());
    }

    return available_devices;
}

// SendIDDSMessage method
int iDDSDevice::SendIDDSMessage(const std::string destination_node_id, const std::string message_)
{
    return m_commandHandler.SendIDDSMessage(destination_node_id, message_);
}

// PrintReceivedIDDSMessages method
void iDDSDevice::PrintReceivedIDDSMessages()
{
    std::cout << "Received iDDS messages: " << std::endl;

    std::vector<Message> veciDDSMessages = m_commandHandler.GetReceivedIDDSMessages();

    for (const auto& msg : veciDDSMessages)
    {
        std::cout << msg.messageBody() << std::endl;
    }
}
