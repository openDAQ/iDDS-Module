#include <idds_wrapper/node_discovery.h>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const char c_node_advertiser_topic[] = "AboutNode";

NodeDiscovery::NodeDiscovery(dds::domain::DomainParticipant& participant,
                             dds::sub::DataReader<AboutNode>& reader,
                             const IddsDeviceInfo& device_info)
    : m_bRunning(false)
    , m_participant(participant)
    , m_device_info(device_info)
    , m_reader(reader)
{
}

NodeDiscovery::~NodeDiscovery()
{
}

void NodeDiscovery::start()
{
    // Start the server
    if(!m_bRunning)
    {
        m_bRunning = true;
        m_nodeDiscoveryThread = std::thread(&NodeDiscovery::beginDiscovery, this);
    }
}

void NodeDiscovery::stop()
{
    if (m_bRunning)
    {
        m_bRunning = false;

        if (m_nodeDiscoveryThread.joinable())
        {
            m_nodeDiscoveryThread.join();
        }
    }
}

/// Method that parses incoming messages to about node topic to discover new iDDS devices
void NodeDiscovery::beginDiscovery()
{
    std::cout << "[iDDS_Wrapper] Node Discovery started" << std::endl;

    while (m_bRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        dds::sub::LoanedSamples<AboutNode> samples;

        /* Try taking samples from the reader. */
        samples = m_reader.take();

        /* Are samples read? */
        if (samples.length() > 0)
        {
            dds::sub::LoanedSamples<AboutNode>::const_iterator sample_iter;
            for (sample_iter = samples.begin(); sample_iter < samples.end(); ++sample_iter)
            {
                /* Get the message and sample information. */
                const AboutNode& msg = sample_iter->data();
                const dds::sub::SampleInfo& info = sample_iter->info();

                // Check if this sample has valid data.
                if (info.valid())
                {
                    const LogicalNodeID& logicalNodeID = msg.logicalNodeID();

                    //Ignore own Logical ID
                    if (logicalNodeID == m_device_info.logical_node_id)
                    {
                        continue;
                    }

                    // Debugging version of unique_id comparison
                    auto it = std::find_if(m_veciDDSNodes.begin(), m_veciDDSNodes.end(),
                             [&logicalNodeID](const AboutNode &node){
                                return node.logicalNodeID() == logicalNodeID;
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

/// Get Available iDDS devices
std::vector<AboutNode> NodeDiscovery::getAvailableIDDSDevices()
{
    return m_veciDDSNodes;
}
