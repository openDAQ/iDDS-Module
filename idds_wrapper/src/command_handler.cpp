#include <idds_wrapper/command_handler.h>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

CommandHandler::CommandHandler(dds::domain::DomainParticipant& participant,
                               const idds_device_info& device_info,
                               std::unordered_map<std::string, message_writer_info>& mapMessageTopics)
    : m_participant(participant)
    , m_bRunning(false)
    , m_device_info(device_info)
    , m_MessageTopic(participant, message_topic_prefix + device_info.logical_node_id) // Topic name includes device's logical node id (eg. "MessageNode1")
    , m_MessageSubscriber(participant)
    , m_MessageReader(m_MessageSubscriber, m_MessageTopic)
    , m_MessagePublisher(participant)
    , m_MessageWriter(m_MessagePublisher, m_MessageTopic)
    , m_mapMessageTopics(mapMessageTopics)
{
}

CommandHandler::~CommandHandler()
{
}

void CommandHandler::Start()
{
    // Start the server
    if(!m_bRunning)
    {
        m_bRunning = true;
        m_commandHandlerThread = std::thread(&CommandHandler::BeginMessageParser, this);
    }
}

void CommandHandler::Stop()
{
    if(m_bRunning)
    {
        m_bRunning = false;

        if (m_commandHandlerThread.joinable())
        {
            m_commandHandlerThread.join();
        }
    }
}

/// Method that parses incoming messages to message topic
void CommandHandler::BeginMessageParser()
{
    std::cout << "[iDDS_Wrapper] Message listener started" << std::endl;

    while (m_bRunning)
    {
        dds::sub::LoanedSamples<Message> samples;

        /* Try taking samples from the reader. */
        samples = m_MessageReader.take();

        /* Are samples read? */
        if (samples.length() > 0)
        {
            dds::sub::LoanedSamples<Message>::const_iterator sample_iter;
            for (sample_iter = samples.begin(); sample_iter < samples.end(); ++sample_iter)
            {
                /* Get the message and sample information. */
                const Message& msg = sample_iter->data();
                const dds::sub::SampleInfo& info = sample_iter->info();

                // Check if this sample has valid data.
                if (info.valid())
                {
                    const LogicalNodeID& logicalNodeID = msg.sourceLogicalNodeID();
                    const std::string& messageBody = msg.messageBody();

                    m_veciDDSMessages.push_back(msg);
                    std::cout << "Received new message from: " << logicalNodeID << "msg: " << messageBody << std::endl;
                }
            }
        }
    }
}

// SendIDDSMessage method
int CommandHandler::SendIDDSMessage(const std::string destination_node_id, const std::string message_)
{
    LogicalNodeID sourceLogicalNodeID{m_device_info.logical_node_id};
    LogicalNodeID targetLogicalNodeID{destination_node_id};
    Time time{0, 0}; // To be adjusteds

    // Retrieve the writer for the target node
    auto it = m_mapMessageTopics.find(destination_node_id);
    if (it == m_mapMessageTopics.end())
    {
        std::cerr << "[iDDS_Wrapper] Error: Destination node not found." << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        Message msg(
            targetLogicalNodeID,            // TargetLogicalNodeID
            sourceLogicalNodeID,            // SourceLogicalNodeID
            message_,                       // Message Body
            0,                              // MyReferenceNumber
            0,                              // YourReferenceNumber
            time,                           // Time
            0,                              // Fragment Number
            false                           // More Fragments
        );

        //write message
        it->second.writer.write(msg);
        return 0;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "[iDDS_Wrapper] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

/// Get Received IDDSMessages
std::vector<Message> CommandHandler::GetReceivedIDDSMessages()
{
    return m_veciDDSMessages;
}
