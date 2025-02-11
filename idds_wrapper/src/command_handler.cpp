#include <idds_wrapper/command_handler.h>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const char message_topic[] = "Message";
//--------------------------------------------------------------------------------------------------

CommandHandler::CommandHandler(dds::domain::DomainParticipant& participant,
                               const idds_device_info& device_info)
    : m_participant(participant)
    , m_bRunning(false)
    , m_device_info(device_info)
    , m_MessageTopic(participant, message_topic)
    , m_MessageSubscriber(participant)
    , m_MessageReader(m_MessageSubscriber, m_MessageTopic)
    , m_MessagePublisher(participant)
    , m_MessageWriter(m_MessagePublisher, m_MessageTopic)
    , m_commandProcessor()
{
    registerCallbacks();
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
                    const LogicalNodeID& logicalNodeID = msg.targetLogicalNodeID();
                    const std::string& messageBody = msg.messageBody();

                    // Only process messages intended for this node
                    if(logicalNodeID == m_device_info.logical_node_id)
                    {
                        m_veciDDSMessages.push_back(msg);

                        std::string response;
                        std::cout << "Received new message from: " << logicalNodeID << " msg: " << messageBody << std::endl;

                        // Check for response message
                        if(parseMessage(msg, response) != idds_wrapper_errCode::METHOD_RESPONSE)
                        {
                            // Reply back to the sender
                            SendIDDSMessage(msg.sourceLogicalNodeID(), response);
                        }
                    }
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
    Time time{0, 0}; // To be adjusted

    try
    {
        Message msg(
            targetLogicalNodeID,            // TargetLogicalNodeID
            sourceLogicalNodeID,            // SourceLogicalNodeID
            message_,                       // Message Body
            0,                              // MyReferenceNumber
            0,                              // YourReferenceNumber
            time,                           // Time
            1,                              // Fragment Number
            false                           // More Fragments
        );

        //write message
        m_MessageWriter.write(msg);
        return 0;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "[iDDS_Wrapper] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

/// Parse incoming messages
idds_wrapper_errCode CommandHandler::parseMessage(const Message& msg, std::string& response)
{
    idds_xml_request parser(msg.messageBody());

    if (parser.parse() == idds_xml_error::ok)
    {
        if(parser.get_return_code() >= 0)
        {
            std::cout << "Reply received: " << msg << std::endl;
            return idds_wrapper_errCode::METHOD_RESPONSE;
        }

        std::cout << "Method name: " << parser.get_method_name() << std::endl;
        std::cout << "Params size: " << parser.get_params().size() << std::endl;

        for (auto param : parser.get_params())
        {
            std::cout << "Param name: " << param << std::endl;
        }

        idds_wrapper_errCode errCode = m_commandProcessor.processCommand(parser.get_method_name(), parser.get_params(), response);
        if (errCode != idds_wrapper_errCode::OK)
        {
            std::cout << "[iDDS_Wrapper] Error processing command" << std::endl;
        }
    }
    else
    {
        std::cout << "[iDDS_Wrapper] Error parsing xml commands" << std::endl;
    }

    return idds_wrapper_errCode::OK;
}

/// Register callbacks in the command processor
void CommandHandler::registerCallbacks()
{
    m_commandProcessor.registerCallback("General.HardReset", [this](const ParamList& params, std::string& response) {
        prepareReply(response, idds_returnCode::OK);
        std::cout << "HardReset command received" << std::endl;
    });

    m_commandProcessor.registerCallback("General.StartOperating", [this](const ParamList& params, std::string& response) {
        prepareReply(response, idds_returnCode::OK);
        std::cout << "StartOperating command received" << std::endl;
    });

    m_commandProcessor.registerCallback("General.StopOperating", [this](const ParamList& params, std::string& response) {
        prepareReply(response, idds_returnCode::OK);
        std::cout << "StopOperating command received" << std::endl;
    });
}

/// Prepare reply
void CommandHandler::prepareReply(std::string& reply, const idds_returnCode returnCode)
{
    idds_xml_response parser = idds_xml_response();
    parser.add_code(static_cast<int>(returnCode));
    auto[result, message] = parser.build();

    if (result == idds_xml_error::ok)
    {
        reply = message;
    }
    else
    {
        std::cout << "[iDDS_Wrapper] Error preparing reply" << std::endl;
    }
}
