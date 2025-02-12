#include <idds_wrapper/node_advertiser.h>
#include <idds_wrapper/idds_state_machine.h>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const char c_node_advertiser_topic[] = "AboutNode";
static const int c_nStreamingDomainID = -1; //Set to -1 (minus one) if not currently known.
static const int c_nAdveritisementInterval = 5; // seconds

NodeAdvertiser::NodeAdvertiser(dds::pub::DataWriter<AboutNode>& writer, const idds_device_info& device_info)
    : m_bRunning(false)
    , m_device_info(device_info)
    , m_writer(writer)
{
}

NodeAdvertiser::~NodeAdvertiser()
{
}

void NodeAdvertiser::start()
{
    // Start the server
    if(!m_bRunning)
    {
        m_bRunning = true;
        m_nodeAdvertiserThread = std::thread(&NodeAdvertiser::beginAdvertise, this);
    }
}

void NodeAdvertiser::stop()
{
    if(m_bRunning)
    {
        m_bRunning = false;
        if (m_nodeAdvertiserThread.joinable())
        {
            m_nodeAdvertiserThread.join();
        }
    }
}

/// Start the node advertiser thread
void NodeAdvertiser::beginAdvertise()
{
    std::cout << "[iDDS_Wrapper] Node advertiser started" << std::endl;

    while (m_bRunning)
    {
        std::this_thread::sleep_for(std::chrono::seconds(c_nAdveritisementInterval));
        if (m_bRunning)
        {
            if(sendAdvertisementMessage() == EXIT_FAILURE)
            {
                std::cout << "[iDDS_Wrapper] Error: Failed to send node advertismente message." << std::endl;
            }
        }
    }
}

// Helper method to send advertisement message
int NodeAdvertiser::sendAdvertisementMessage()
{
    RealNodeID realNodeID{m_device_info.manufacturer, m_device_info.productType, m_device_info.serialNumber};
    BuildStandard buildStandard{m_device_info.hwVersion, m_device_info.swVersion};
    LogicalNodeID logicalNodeID{m_device_info.logical_node_id};
    Time aboutTime{0, 0}; // To be adjusted

    // Get current state
    IDDSStateMachine &machine = IDDSStateMachine::getInstance();

    try
    {
        AboutNode msg(
            realNodeID,                             // RealNodeID
            buildStandard,                          // BuildStandard
            machine.getState(),                     // To be adjusted
            "",                                     // StatusReason - To be adjusted
            logicalNodeID,                          // LogicalNodeID
            c_nStreamingDomainID,                   // DomainID
            m_device_info.ipAddress,                // IPAddress
            aboutTime                               // Time
        );

        m_writer.write(msg);
        return 0;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "[iDDS_Wrapper] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}