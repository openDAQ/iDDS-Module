#include <idds_wrapper/node_discovery.h>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const char c_node_advertiser_topic[] = "AboutNode";

NodeDiscovery::NodeDiscovery(dds::domain::DomainParticipant& participant,
                             dds::sub::DataReader<AboutNode>& reader,
                             const idds_device_info& device_info,
                             std::unordered_map<std::string, message_writer_info>& mapMessageTopics)
    : m_bRunning(false)
    , m_participant(participant)
    , m_device_info(device_info)
    , m_reader(reader)
    , m_mapMessageTopics(mapMessageTopics)
{
}

NodeDiscovery::~NodeDiscovery()
{
}

void NodeDiscovery::Start()
{
    // Start the server
    if(!m_bRunning)
    {
        m_bRunning = true;
        m_nodeDiscoveryThread = std::thread(&NodeDiscovery::BeginDiscovery, this);
    }
}

void NodeDiscovery::Stop()
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
void NodeDiscovery::BeginDiscovery()
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
                        CreateTopic(logicalNodeID);

                        std::cout << "New iDDS Device Found: " << msg << std::endl;
                    }
                }
            }
        }
    }
}

void NodeDiscovery::CreateTopic(const std::string logicalNodeID)
{
    std::string topic_name = message_topic_prefix + logicalNodeID;

    // Topic creation
    auto MessageTopic = dds::topic::Topic<Message>(m_participant, topic_name);
    auto publisher = dds::pub::Publisher(m_participant);
    auto writer = dds::pub::DataWriter<Message>(publisher, MessageTopic);

    // tbd: Needs thread safety
    message_writer_info writer_info(MessageTopic, publisher, writer);
    m_mapMessageTopics.insert({logicalNodeID, writer_info});
}

/// Get Available iDDS devices
std::vector<AboutNode> NodeDiscovery::GetAvailableIDDSDevices()
{
    return m_veciDDSNodes;
}
