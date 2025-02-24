#include <idds_wrapper/idds_device_impl.h>
#include <iostream>
#include <fstream>
#include <filesystem>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------

static const int c_nDomainID = 0;
static const int c_nStreamingDomainID = 2;
static const char node_advertiser_topic[] = "AboutNode";
static const char message_topic[] = "Message";
static const char rtps_file[] = "rtps.ini";

iDDSDevice::iDDSDevice(const std::string node_id, const std::string manufacturer, const std::string productType,
                       const std::string serialNumber, const std::string hwVersion, const std::string swVersion,
                       const std::string ipAddress)
                       : m_idds_device_info{node_id, manufacturer, productType,
                                serialNumber, hwVersion, swVersion, ipAddress}
                       // cycloneDDS specific initialization
                       , m_participant(c_nDomainID)
                       , m_streamParticipant(c_nStreamingDomainID)
                       , m_aboutNodeTopic(m_participant, node_advertiser_topic)
                       , m_aboutNodesubscriber(m_participant)
                       , m_aboutNodeReader(m_aboutNodesubscriber, m_aboutNodeTopic)
                       , m_aboutNodepublisher(m_participant)
                       , m_aboutNodewriter(m_aboutNodepublisher, m_aboutNodeTopic)
                       , m_ParameterDataSeriesSubscriber(m_streamParticipant, m_streamParticipant.default_subscriber_qos() << dds::core::policy::Partition("default"))
                       , m_ParameterDataSeriesPublisher(m_streamParticipant, m_streamParticipant.default_publisher_qos() << dds::core::policy::Partition("default"))
                       , m_nodeAdvertiser(m_aboutNodewriter, m_idds_device_info)
                       , m_nodeDiscovery(m_participant, m_aboutNodeReader, m_idds_device_info)
                       , m_channelStreamer(m_streamParticipant, m_idds_device_info, m_ParameterDataSeriesPublisher, m_ParameterDataSeriesSubscriber)
                       , m_commandHandler(m_participant, m_idds_device_info, m_channelStreamer)
{
}

// Destructor
iDDSDevice::~iDDSDevice()
{
    // Join threads if they are joinable
    if (m_bRunning)
    {
        stopServer();
    }
}

// startServer method
void iDDSDevice::startServer()
{
    // Start the server
    m_bRunning = true;

    m_nodeAdvertiser.start();
    m_nodeDiscovery.start();
    m_commandHandler.start();
    m_channelStreamer.startStreamer();
    m_channelStreamer.startReaderThread();
}

// Subscribe To a channel for streaming data
void iDDSDevice::subscribeToChannel(const std::string channelName)
{
    m_channelStreamer.subscribeToChannel(channelName);
}

// Subscribe To a channel for streaming data
void iDDSDevice::subscribeToChannel(const int channelParamID)
{
    m_channelStreamer.subscribeToChannel(channelParamID);
}

// Stop the iDDS server
void iDDSDevice::stopServer()
{
    m_bRunning = false;

    m_nodeAdvertiser.stop();
    m_nodeDiscovery.stop();
    m_commandHandler.stop();
}

// getAvailableIDDSDevices method
std::vector<std::string> iDDSDevice::getAvailableIDDSDevices()
{
    std::vector<std::string> available_devices;

    std::vector<AboutNode> veciDDSNodes = m_nodeDiscovery.getAvailableIDDSDevices();

    for (auto &node : veciDDSNodes)
    {
        available_devices.push_back(node.logicalNodeID());
    }

    return available_devices;
}

// sendIDDSMessage method
IddsWrapperErrCode iDDSDevice::sendIDDSMessage(const std::string& destination_node_id, const std::string& message)
{
    return m_commandHandler.sendIDDSMessage(destination_node_id, message);
}

// Request command
IddsWrapperErrCode iDDSDevice::publishCommand(const std::string& targetLogicalNodeID, const std::string& command)
{
    auto strCommandXml = m_commandHandler.getCommandXML(command);

    if(strCommandXml.empty())
        return IddsWrapperErrCode::ERROR;
    else
    {
        m_commandHandler.publishCommandAndWaitForReply(targetLogicalNodeID, strCommandXml);
        return IddsWrapperErrCode::OK;
    }
}


/// Get Available Signal Name and ID
std::map<std::string, ParameterID> iDDSDevice::getDiscoverableSignalNameAndID()
{
    return m_channelStreamer.getDiscoverableSignalNameAndID();
}

// printReceivedIDDSMessages method
void iDDSDevice::printReceivedIDDSMessages()
{
    std::cout << "Received iDDS messages: " << std::endl;

    std::vector<Message> veciDDSMessages = m_commandHandler.getReceivedIDDSMessages();

    for (const auto& msg : veciDDSMessages)
    {
        std::cout << msg.messageBody() << std::endl;
    }
}
