#include <idds_wrapper/iDDSDevice.h>
#include <iostream>
#include <fstream>
#include <filesystem>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const int c_nAdveritisementInterval = 1; // seconds
static const int c_nDomainID = 0;
static const int c_nStreamingDomainID = -1; //Set to -1 (minus one) if not currently known.
static const char node_advertiser_topic[] = "AboutNode";
static const char message_topic[] = "Message";
static const char rtps_file[] = "rtps.ini";


iDDSDevice::iDDSDevice(const std::string node_id, const std::string manufacturer, const std::string productType,
                       const std::string serialNumber, const std::string hwVersion, const std::string swVersion,
                       const std::string ipAddress)
                       : node_id(node_id)
                       , manufacturer(manufacturer)
                       , productType(productType)
                       , serialNumber(serialNumber)
                       , hwVersion(hwVersion)
                       , swVersion(swVersion)
                       , ipAddress(ipAddress)
                       // cycloneDDS specific initialization
                       , participant(c_nDomainID)
                       , m_aboutNodeTopic(participant, node_advertiser_topic)
                       , m_aboutNodesubscriber(participant)
                       , m_aboutNodeReader(m_aboutNodesubscriber, m_aboutNodeTopic)
                       , m_aboutNodepublisher(participant)
                       , m_aboutNodewriter(m_aboutNodepublisher, m_aboutNodeTopic)
                       , m_MessageTopic(participant, message_topic)
                       , m_MessageSubscriber(participant)
                       , m_MessageReader(m_MessageSubscriber, m_MessageTopic)
                       , m_MessagePublisher(participant)
                       , m_MessageWriter(m_MessagePublisher, m_MessageTopic)
{
}

// Destructor
iDDSDevice::~iDDSDevice()
{
    // Join threads if they are joinable
    if (m_bRunning)
    {
        m_bRunning = false;

        if (advertiser_thread.joinable())
        {
            advertiser_thread.join();
        }

        if (node_discovery_thread.joinable())
        {
            node_discovery_thread.join();
        }

        if (message_listener_thread.joinable())
        {
            message_listener_thread.join();
        }
    }
}

// StartServer method
void iDDSDevice::StartServer()
{
    // Start the server
    m_bRunning = true;

    advertiser_thread = std::thread(&iDDSDevice::NodeAdvertiser, this);
    node_discovery_thread = std::thread(&iDDSDevice::NodeDiscovery, this);
    message_listener_thread = std::thread(&iDDSDevice::MessageListener, this);
}

// GetAvailableIDDSDevices method
std::vector<std::string> iDDSDevice::GetAvailableIDDSDevices()
{
    std::vector<std::string> available_devices;

    for (auto &node : m_veciDDSNodes)
    {
        available_devices.push_back(node.aboutLogicalNodeID());
    }

    return available_devices;
}

// SendIDDSMessage method
int iDDSDevice::SendIDDSMessage(const std::string destination_node_id, const std::string message_)
{
    iDDS::LogicalNodeID sourceLogicalNodeID{node_id};
    iDDS::LogicalNodeID targetLogicalNodeID{destination_node_id};
    iDDS::Time time{0, 0}; // To be adjusted

    try
    {
        iDDS::Message msg(
            targetLogicalNodeID,            // TargetLogicalNodeID
            sourceLogicalNodeID,            // SourceLogicalNodeID
            message_,                       // Message Body
            0,                              // MyReferenceNumber
            0,                              // YourReferenceNumber
            time,                           // Time
            0,                              // Fragment Number
            false                           // More Fragments
        );

        m_MessageWriter.write(msg);
        return 0;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "[iDDS_Wrapper] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

// PrintReceivedIDDSMessages method
void iDDSDevice::PrintReceivedIDDSMessages()
{
    std::cout << "Received iDDS messages: " << std::endl;
    for (const auto& msg : m_veciDDSMessages)
    {
        std::cout << msg.messageBody() << std::endl;
    }
}

// NodeAdvertiser method
void iDDSDevice::NodeAdvertiser()
{
    // Placeholder implementation
    std::cout << "[iDDS_Wrapper] Node advertiser started" << std::endl;

    while (m_bRunning)
    {
        std::this_thread::sleep_for(std::chrono::seconds(c_nAdveritisementInterval));
        if (m_bRunning)
        {
            if(SendAdvertisementMessage() == EXIT_FAILURE)
            {
                std::cout << "[iDDS_Wrapper] Error: Failed to send node advertismente message." << std::endl;
            }
        }
    }
}

// Method that parses incoming messages to about node topic to discover new iDDS devices
void iDDSDevice::NodeDiscovery()
{
    std::cout << "[iDDS_Wrapper] Node Discovery started" << std::endl;

    while (m_bRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        dds::sub::LoanedSamples<iDDS::AboutNode> samples;

        /* Try taking samples from the reader. */
        samples = m_aboutNodeReader.take();

        /* Are samples read? */
        if (samples.length() > 0)
        {
            dds::sub::LoanedSamples<iDDS::AboutNode>::const_iterator sample_iter;
            for (sample_iter = samples.begin(); sample_iter < samples.end(); ++sample_iter)
            {
                /* Get the message and sample information. */
                const iDDS::AboutNode& msg = sample_iter->data();
                const dds::sub::SampleInfo& info = sample_iter->info();

                // Check if this sample has valid data.
                if (info.valid())
                {
                    const iDDS::LogicalNodeID& logicalNodeID = msg.aboutLogicalNodeID();

                    //Ignore own Logical ID
                    if (logicalNodeID == node_id)
                    {
                        continue;
                    }

                    // Debugging version of unique_id comparison
                    auto it = std::find_if(m_veciDDSNodes.begin(), m_veciDDSNodes.end(),
                             [&logicalNodeID](const iDDS::AboutNode &node){
                                return node.aboutLogicalNodeID() == logicalNodeID;
                             });

                    // If unique_id is not found, append the new message to the vector
                    if (it == m_veciDDSNodes.end())
                    {
                        m_veciDDSNodes.push_back(msg);

                        std::cout << "New iDDS Device Found: " << msg << std::endl;
                    }
                }
            }
        }
    }
}

// Method that parses incoming messages to message topic
void iDDSDevice::MessageListener()
{
    std::cout << "[iDDS_Wrapper] Message listener started" << std::endl;

    while (m_bRunning)
    {
        dds::sub::LoanedSamples<iDDS::Message> samples;

        /* Try taking samples from the reader. */
        samples = m_MessageReader.take();

        /* Are samples read? */
        if (samples.length() > 0)
        {
            dds::sub::LoanedSamples<iDDS::Message>::const_iterator sample_iter;
            for (sample_iter = samples.begin(); sample_iter < samples.end(); ++sample_iter)
            {
                /* Get the message and sample information. */
                const iDDS::Message& msg = sample_iter->data();
                const dds::sub::SampleInfo& info = sample_iter->info();

                // Check if this sample has valid data.
                if (info.valid())
                {
                    const iDDS::LogicalNodeID& logicalNodeID = msg.sourceLogicalNodeID();
                    const std::string& messageBody = msg.messageBody();

                    m_veciDDSMessages.push_back(msg);
                    std::cout << "Received new message from: " << logicalNodeID << "msg: " << messageBody << std::endl;
                }
            }
        }
    }
}

// Helper method to send advertisement message
int iDDSDevice::SendAdvertisementMessage()
{
    iDDS::RealNodeID realNodeID{manufacturer, productType, serialNumber};
    iDDS::BuildStandard buildStandard{hwVersion, swVersion};
    iDDS::LogicalNodeID logicalNodeID{node_id};
    iDDS::Time aboutTime{0, 0}; // To be adjusted

    try
    {
        iDDS::AboutNode msg(
            realNodeID,                             // RealNodeID
            buildStandard,                          // BuildStandard
            iDDS::OperationalStatus::OpStatusReady, // To be adjusted
            "",                                     // StatusReason - To be adjusted
            logicalNodeID,                          // LogicalNodeID
            c_nStreamingDomainID,                   // DomainID
            ipAddress,                              // IPAddress
            aboutTime                               // Time
        );

        m_aboutNodewriter.write(msg);
        return 0;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "[iDDS_Wrapper] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}