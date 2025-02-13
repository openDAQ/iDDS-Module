#include <idds_wrapper/command_handler.h>
#include <idds_wrapper/idds_state_machine.h>

#include <tuple>
#include <regex>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const char c_message_topic[] = "Message";
static constexpr int c_timeout_ms   = 10000;             // Timeout for reply (milliseconds)
//--------------------------------------------------------------------------------------------------


dds::sub::qos::DataReaderQos getMessageReaderQoSFlags(dds::topic::Topic<Message>& topic)
{
    dds::sub::qos::DataReaderQos currQos = topic.qos();

    std::vector<dds::core::policy::DataRepresentationId> reprs;
    reprs.push_back(dds::core::policy::DataRepresentationId::XCDR1);
    currQos << dds::core::policy::DataRepresentation(reprs);

    currQos << dds::core::policy::LatencyBudget(dds::core::Duration(0, 100000));

    currQos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind_def::RELIABLE, dds::core::Duration(0, 100000000));

    return currQos;
}

CommandHandler::CommandHandler(dds::domain::DomainParticipant& participant,
                               const idds_device_info& device_info,
                               ChannelStreamer& channelStreamer)
    : m_participant(participant)
    , m_bRunning(false)
    , m_device_info(device_info)
    , m_MessageTopic(participant, c_message_topic)
    , m_MessageSubscriber(participant)
    , m_MessageReader(m_MessageSubscriber, m_MessageTopic, getMessageReaderQoSFlags(m_MessageTopic))
    , m_MessagePublisher(participant)
    , m_MessageWriter(m_MessagePublisher, m_MessageTopic)
    , m_commandProcessor()
    , m_channelStreamer(channelStreamer)
{
    registerCallbacks();
}

CommandHandler::~CommandHandler()
{
}

void CommandHandler::start()
{
    // Start the server
    if(!m_bRunning)
    {
        m_bRunning = true;
        m_commandHandlerThread = std::thread(&CommandHandler::BeginMessageParser, this);
    }
}

void CommandHandler::stop()
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
                        std::cout << "New message from: " << msg.sourceLogicalNodeID() << " msg: " << messageBody << std::endl;

                        // Check if its a response message
                        if(std::string(msg.messageBody()).find("<methodResponse>") != std::string::npos)
                        {
                            // Process response message
                            setReplyAvailable();
                            m_strReply = msg.messageBody();
                        }
                        else
                        {
                            if(parseMessage(msg, response) != idds_wrapper_errCode::OK)
                            {
                                std::cerr << "[iDDS_Wrapper] Error processing message" << std::endl;
                            }

                            sendIDDSMessage(msg.sourceLogicalNodeID(), response);
                        }
                    }
                }
            }
        }
    }
}

// sendIDDSMessage method
idds_wrapper_errCode CommandHandler::sendIDDSMessage(const std::string destination_node_id, const std::string message_)
{
    LogicalNodeID sourceLogicalNodeID{m_device_info.logical_node_id};
    LogicalNodeID targetLogicalNodeID{destination_node_id};

    uint32_t seconds_since_epoch = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
    uint32_t nanoseconds_since_epoch = std::chrono::system_clock::now().time_since_epoch() / std::chrono::nanoseconds(1);
    Time time{seconds_since_epoch, nanoseconds_since_epoch};

    try
    {
        Message msg(
            targetLogicalNodeID,            // TargetLogicalNodeID
            sourceLogicalNodeID,            // SourceLogicalNodeID
            message_,                       // Message Body
            0,                              // MyReferenceNumber
            0,                              // YourReferenceNumber
            time,                           // Time
            1,                              // Fragment Number: 1 fragment
            false                           // More Fragments
        );

        //write message
        m_MessageWriter.write(msg);
        return idds_wrapper_errCode::OK;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "[iDDS_Wrapper] Exception: " << e.what() << std::endl;
        return idds_wrapper_errCode::NOK;
    }
}

/// Parse incoming messages
idds_wrapper_errCode CommandHandler::parseMessage(const Message& msg, std::string& response)
{
    idds_xml_request parser(msg.messageBody());

    if (parser.parse() == idds_xml_error::ok)
    {
        std::cout << "Method name: " << parser.get_method_name() << std::endl;

        idds_wrapper_errCode errCode = m_commandProcessor.processCommand(parser.get_method_name(), parser.get_params(), response);
        if (errCode != idds_wrapper_errCode::OK)
        {
            std::cout << "[iDDS_Wrapper] Error processing command " << parser.get_method_name() << std::endl;
            prepareReply(response, translateReturnCode(errCode));
            return errCode;
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
    // General.HardReset
    m_commandProcessor.registerCallback("General.HardReset", [this](const ParamList& params, std::string& response) {
        // Do nothing - TBD
        prepareReply(response, idds_returnCode::OK);
    });

    // Configuration.GetAttribute
    m_commandProcessor.registerCallback("Configuration.GetAttribute", [this](const ParamList& params, std::string& response) {
        std::cout << "GetAttribute command received" << std::endl;

        std::string value;
        idds_xml_error result;

        for (auto param : params)
        {
            idds_xml_params_decode<std::string> idds_param = idds_xml_params_decode<std::string>(param);
            std::tie(result, value) = idds_param.get_name();
            std::cout << "Param name: " << value << std::endl;
             std::tie(result, value) = idds_param.get_value();
            std::cout << "Param value: " << value << std::endl;
        }

        if(value == "alistv")
        {
            // Channel information requested
            std::string strChannelInfo;

            // Prepare channel information response
            std::string strParam = prepareXMLResponse( m_channelStreamer.getChannelInfo());
            prepareReply(response, idds_returnCode::OK, {strParam});
        }
        else
        {
            // Anything else is not supported at this moment
            prepareReply(response, idds_returnCode::CommandNotSupported);
        }
    });

    // General.StartOperating
    m_commandProcessor.registerCallback("General.StartOperating", [this](const ParamList& params, std::string& response) {

        std::cout << "StartOperating command received" << std::endl;
        std::cout << "State: " << IDDSStateMachine::getInstance().getState() << std::endl;
        IDDSStateMachine::getInstance().setState(OperationalStatus::OpStatusOperating);
        std::cout << "State: " << IDDSStateMachine::getInstance().getState() << std::endl;

        prepareReply(response, idds_returnCode::OK);
    });

    // General.StopOperating
    m_commandProcessor.registerCallback("General.StopOperating", [this](const ParamList& params, std::string& response) {
        std::cout << "StopOperating command received" << std::endl;

        std::cout << "State: " << IDDSStateMachine::getInstance().getState() << std::endl;
        IDDSStateMachine::getInstance().setState(OperationalStatus::OpStatusReady);
        std::cout << "State: " << IDDSStateMachine::getInstance().getState() << std::endl;

        prepareReply(response, idds_returnCode::OK);
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

/// Prepare reply and include parameters in the response
void CommandHandler::prepareReply(std::string& reply, const idds_returnCode returnCode, std::vector<std::string> param)
{
    idds_xml_response parser = idds_xml_response();
    parser.add_code(static_cast<int>(returnCode));
    for (auto p : param)
    {
        parser.add_param(p);
    }
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

// Translate idds-return_code to idds_wrapper_errCode
idds_returnCode CommandHandler::translateReturnCode(const idds_wrapper_errCode returnCode)
{
    switch (returnCode)
    {
    case idds_wrapper_errCode::OK:
        return idds_returnCode::OK;
    case idds_wrapper_errCode::NOK:
    case idds_wrapper_errCode::CALLBACK_NOT_FOUND:
    default:
        return idds_returnCode::CommandNotSupported;
    }
}

/// Prepare XML response to be sent back after GetAttribute command
std::string CommandHandler::prepareXMLResponse(std::string value)
{
    std::string strParam = "<param><name>Attribute</name><value><String>";
    strParam += value;
    strParam += "</String></value></param>";
    return strParam;
}

// Method to be called by another thread when a reply is available
void CommandHandler::setReplyAvailable()
{
        std::lock_guard<std::mutex> lock(mtx);
        m_bReplyAvailable = true;
        cv.notify_one();
}

// Publishes a command and waits for a reply or timeout
idds_wrapper_errCode CommandHandler::publishCommandAndWaitForReply(const std::string &destination_node_id, const std::string &message_)
{
    std::unique_lock<std::mutex> lock(mtx);
    m_bReplyAvailable = false; // Reset flag before sending message

    // Send message
    auto errCode = sendIDDSMessage(destination_node_id, message_);
    if (errCode != idds_wrapper_errCode::OK)
    {
        std::cerr << "Failed to send message!" << std::endl;
        return errCode;
    }

    // Wait for reply or timeout
    bool receivedReply = cv.wait_for(lock, std::chrono::milliseconds(c_timeout_ms), [this]()
                                     { return m_bReplyAvailable; });

    if (receivedReply)
    {
        // Process reply
        m_bReplyAvailable = false;
        // Check if it has params
        idds_xml_response parser = idds_xml_response(m_strReply);
        if (parser.parse() == idds_xml_error::ok)
        {
            if(parser.get_params().size() > 0)
            {
                idds_xml_params_decode<std::string> stringDecode(parser.get_params()[0]);
                auto [err, value] = stringDecode.get_value();
                if(err == idds_xml_error::ok)
                {
                    extractChannelNameAndID(value);
                }
            }
        }
        else
        {
            std::cerr << "[iDDS_Wrapper] Error parsing reply" << std::endl;
        }
    }
    else
    {
        std::cerr << "[iDDS_Wrapper] Timeout error: No reply received!" << std::endl;
        return idds_wrapper_errCode::RESPONSE_TIMEOUT;
    }

    return idds_wrapper_errCode::OK;
}

void CommandHandler::extractChannelNameAndID(const std::string& input)
{
    std::regex nameRegex(R"(\{(.*?)\}\{(.*?)\}\{Name\}\{(.*?)\})");
    std::regex idRegex(R"(\{(.*?)\}\{(.*?)\}\{ID\}\{(.*?)\})");

    std::sregex_iterator nameIt(input.begin(), input.end(), nameRegex);
    std::sregex_iterator idIt(input.begin(), input.end(), idRegex);
    std::sregex_iterator end;

    while (nameIt != end && idIt != end)
    {
        std::string name = (*nameIt)[3];
        std::string id = (*idIt)[3];

        m_channelStreamer.addDiscoverableChannel(name, std::stoi(id));

        ++nameIt;
        ++idIt;
    }
}
