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
                       const std::string ipAddress) : node_id(node_id), manufacturer(manufacturer), productType(productType),
                                                      serialNumber(serialNumber), hwVersion(hwVersion), swVersion(swVersion),
                                                      ipAddress(ipAddress),
                                                      listenerNodeAdvertisement_impl(new DataReaderListenerImpl),
                                                      listenerNodeAdvertisement(listenerNodeAdvertisement_impl),
                                                      listenerCommand_impl(new CommandListenerImpl),
                                                      listenerCommand(listenerCommand_impl)
{
    SetupiDDSDevice();
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
    }

    // Clean up
    if(participant)
    {
        participant->delete_contained_entities();
    }

    if(dpf)
    {
        dpf->delete_participant(participant);
    }

    TheServiceParticipant->shutdown();
}

/// Initialize Domain Factory and Participant
int iDDSDevice::SetupiDDSDevice()
{

    try
    {
        // Check RTPS ini file exists
        std::ifstream file(rtps_file);
        if (!file.good())
        {
            // Get current working directory
            std::string cwd = std::filesystem::current_path().string();
            std::string full_path = cwd + "/" + rtps_file;

            ACE_ERROR((LM_ERROR,
                                  ACE_TEXT("ERROR: RTPS %s does not exist!\n"), full_path.c_str()));
            return -1;
        }

        // Initialize DomainParticipantFactory
        TheServiceParticipant->default_configuration_file(ACE_TEXT(rtps_file));
        DDS::DomainParticipantFactory_var dpf = TheServiceParticipant->get_domain_participant_factory();

        if (!dpf)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT("Domain Pariticipant Factory failed! Please check Environment variables are set.\n")),
                             1);

            return -1;
        }

        // Create DomainParticipant
        participant =
            dpf->create_participant(c_nDomainID,
                                    PARTICIPANT_QOS_DEFAULT,
                                    nullptr,
                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (!participant)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_participant failed!\n")),
                             1);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught setting up iDDS device" << std::endl;
        return -1;
    }

    CreateNodeAdvertiserTopic();
    CreateMessageTopic();

    return 0;
}

/// Create Node Advertiser Topic
int iDDSDevice::CreateNodeAdvertiserTopic()
{
    try
    {
        // Register TypeSupport (RealTimeBackbone::AboutNode)
        RealTimeBackbone::AboutNodeTypeSupport_var ts =
            new RealTimeBackbone::AboutNodeTypeSupportImpl;

        if (ts->register_type(participant, "") != DDS::RETCODE_OK)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: CreateNodeAdvertiserTopic() -")
                                  ACE_TEXT(" register_type failed!\n")),
                             1);
        }

        // Create Topic
        CORBA::String_var type_name = ts->get_type_name();
        NodeAdvertiserTopic =
            participant->create_topic(node_advertiser_topic,
                                      type_name,
                                      TOPIC_QOS_DEFAULT,
                                      0,
                                      OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!NodeAdvertiserTopic)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: CreateNodeAdvertiserTopic() -")
                                  ACE_TEXT(" create_topic failed!\n")),
                             1);
        }

        // Create Publisher
        NodeAdvertiserPublisher =
            participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                          0,
                                          OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!NodeAdvertiserPublisher)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: CreateNodeAdvertiserTopic() -")
                                  ACE_TEXT(" create_publisher failed!\n")),
                             1);
        }

        // Set QoS for DataWriter
        DDS::DataWriterQos writer_qos;
        NodeAdvertiserPublisher->get_default_datawriter_qos(writer_qos);
        writer_qos.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;

        NodeAdvertiserWriter =
            NodeAdvertiserPublisher->create_datawriter(NodeAdvertiserTopic,
                                         writer_qos,
                                         nullptr,
                                         OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!NodeAdvertiserWriter)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: CreateNodeAdvertiserTopic() -")
                                  ACE_TEXT(" create_datawriter failed!\n")),
                             1);
        }

        AboutNode_writer =
            RealTimeBackbone::AboutNodeDataWriter::_narrow(NodeAdvertiserWriter);

        if (!AboutNode_writer)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: CreateNodeAdvertiserTopic() -")
                                  ACE_TEXT(" _narrow failed!\n")),
                             1);
        }

        // Create Subscriber
        NodeAdvertiserSubscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                       0,
                                       OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!NodeAdvertiserSubscriber)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: CreateNodeAdvertiserTopic() -")
                              ACE_TEXT(" create_subscriber failed!\n")),
                         1);
        }

        // Set QoS for DataReader
        DDS::DataReaderQos reader_qos;
        NodeAdvertiserSubscriber->get_default_datareader_qos(reader_qos);
        reader_qos.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;

        NodeAdvertiserReader =
            NodeAdvertiserSubscriber->create_datareader(NodeAdvertiserTopic,
                                      reader_qos,
                                      listenerNodeAdvertisement,
                                      OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!NodeAdvertiserReader)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: CreateNodeAdvertiserTopic() -")
                                  ACE_TEXT(" create_datareader failed!\n")),
                             1);
        }

        return 0;
    }
    catch (const CORBA::Exception &e)
    {
        e._tao_print_exception("Exception caught creating node advertisement topic");
        return 1;
    }
}

// Create Message Topic
int iDDSDevice::CreateMessageTopic()
{
    try
    {
        // Register TypeSupport (RealTimeBackbone::iDDSControlMsg)
        RealTimeBackbone::MessageTypeSupport_var ts =
            new RealTimeBackbone::MessageTypeSupportImpl;

        if (ts->register_type(participant, "") != DDS::RETCODE_OK)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" register_type failed!\n")),
                             1);
        }

        // Create Topic
        CORBA::String_var type_name = ts->get_type_name();
        MessageTopic = participant->create_topic(message_topic,
                                                 type_name,
                                                 TOPIC_QOS_DEFAULT,
                                                 nullptr,
                                                 OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!MessageTopic)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_topic failed!\n")),
                             1);
        }

        // Create Publisher
        MessagePublisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                                         nullptr,
                                                         OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!MessagePublisher)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_publisher failed!\n")),
                             1);
        }

        // Set QoS for DataWriter
        DDS::DataWriterQos writer_qos;
        MessagePublisher->get_default_datawriter_qos(writer_qos);

        writer_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
        writer_qos.durability.kind = DDS::VOLATILE_DURABILITY_QOS;
        writer_qos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
        writer_qos.history.depth = 10;

        MessageWriter = MessagePublisher->create_datawriter(MessageTopic,
                                                            writer_qos,
                                                            nullptr,
                                                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!MessageWriter)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_datawriter failed!\n")),
                             1);
        }

        Message_writer = RealTimeBackbone::MessageDataWriter::_narrow(MessageWriter);

        if (!Message_writer)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" _narrow failed!\n")),
                             1);
        }

        // Create Subscriber
        MessageSubscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                                           nullptr,
                                                           OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!MessageSubscriber)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_subscriber failed!\n")),
                             1);
        }

        DDS::DataReaderQos reader_qos;
        MessageSubscriber->get_default_datareader_qos(reader_qos);


        reader_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
        reader_qos.durability.kind = DDS::VOLATILE_DURABILITY_QOS;
        reader_qos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
        reader_qos.history.depth = 10;

        DDS::DataReader_var reader =
            MessageSubscriber->create_datareader(MessageTopic,
                                                 reader_qos,
                                                 listenerCommand,
                                                 OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!reader)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_datareader failed!\n")),
                             1);
        }

        return 0;
    }
    catch (const CORBA::Exception &e)
    {
        e._tao_print_exception("Exception caught creating message topic");
        return 1;
    }
}


// StartServer method
void iDDSDevice::StartServer()
{
    // Start the server
    m_bRunning = true;

    advertiser_thread = std::thread(&iDDSDevice::NodeAdvertiser, this);
}

// GetAvailableIDDSDevices method
std::vector<std::string> iDDSDevice::GetAvailableIDDSDevices()
{
    std::vector<RealTimeBackbone::AboutNode> vec = listenerNodeAdvertisement_impl->get_message_vector();
    std::vector<std::string> available_devices;

    for (const auto &msg : vec)
    {
        std::string strRealNodeId = (std::string) msg.logicalNodeID.in();
        available_devices.push_back(strRealNodeId);
    }

    // Placeholder implementation
    return available_devices;
}

// SendIDDSMessage method
int iDDSDevice::SendIDDSMessage(const std::string destination_node_id, const std::string message_)
{
    try
    {
        if(!MessageWriter)
        {
            ACE_ERROR((LM_ERROR,
                              ACE_TEXT("ERROR: MessageWriter is null!\n")));

            return -1;
        }

        // Block until Subscriber is available
        DDS::StatusCondition_var condition = MessageWriter->get_statuscondition();
        condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);

        DDS::WaitSet_var ws = new DDS::WaitSet;
        ws->attach_condition(condition);

        //ACE_DEBUG((LM_DEBUG,
        //           ACE_TEXT("Block until subscriber is available\n")));

        while (true)
        {
            DDS::PublicationMatchedStatus matches;
            if (MessageWriter->get_publication_matched_status(matches) != ::DDS::RETCODE_OK)
            {
                ACE_ERROR_RETURN((LM_ERROR,
                                  ACE_TEXT("ERROR: %N:%l: main() -")
                                      ACE_TEXT(" get_publication_matched_status failed!\n")),
                                 1);
            }

            if (matches.current_count >= 1)
            {
                break;
            }

            DDS::ConditionSeq conditions;
            DDS::Duration_t timeout = {60, 0};
            if (ws->wait(conditions, timeout) != DDS::RETCODE_OK)
            {
                ACE_ERROR_RETURN((LM_ERROR,
                                  ACE_TEXT("ERROR: %N:%l: main() -")
                                      ACE_TEXT(" wait failed!\n")),
                                 1);
            }
        }

        //ACE_DEBUG((LM_DEBUG,
        //           ACE_TEXT("Subscriber is available\n")));

        ws->detach_condition(condition);

        // Write samples
        RealTimeBackbone::Message message;
        message.targetLogicalNodeID = destination_node_id.c_str();
        message.sourceLogicalNodeID = node_id.c_str();

        // Make sure the string length does not exceed the max length
        if (message_.size() > RealTimeBackbone::MAXIMUM_MESSAGE_BODY_LENGTH)
        {
            // Handle the error: string is too long to fit into the iDDSCommand
            std::cerr << "Error: Command string is too long!" << std::endl;
            return 1;
        }
        else
        {
            message.messageBody = message_.c_str();
        }

        message.myReferenceNumber = 0;
        message.yourReferenceNumber = 0;
        message.time.seconds = 0;
        message.time.nanoseconds = 0;
        message.fragmentNumber = 0;
        message.moreFragments = false;

        DDS::ReturnCode_t error = Message_writer->write(message, DDS::HANDLE_NIL);

        if (error != DDS::RETCODE_OK)
        {
            ACE_ERROR((LM_ERROR,
                       ACE_TEXT("ERROR: %N:%l: main() -")
                           ACE_TEXT(" write returned %d!\n"),
                       error));
        }

        // Wait for samples to be acknowledged
        DDS::Duration_t timeout = {30, 0};
        if (Message_writer->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" wait_for_acknowledgments failed!\n")),
                             1);
        }

        return 0;
    }
    catch (const CORBA::Exception &e)
    {
        e._tao_print_exception("Exception caught sending iDDS message");
        return 1;
    }
}

// PrintReceivedIDDSMessages method
void iDDSDevice::PrintReceivedIDDSMessages()
{
    // Placeholder implementation
    int i = 0;

    std::vector<RealTimeBackbone::Message> vec = listenerCommand_impl->get_message_vector();

    for (RealTimeBackbone::Message msg : vec)
    {
        std::cout << "#" << i++ << " - ";
        std::cout << "from: = " << msg.sourceLogicalNodeID;
        std::cout << "; to: = " << msg.targetLogicalNodeID << std::endl;

        // Printing the message
        if (static_cast<std::string>(msg.messageBody).size() > 0) {
            std::cout << "message: = " << msg.messageBody << std::endl;
        }
        else
        {
            std::cout << "message: = [empty]" << std::endl;
        }
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
            if(SendAdvertisementMessage() == -1)
            {
                ACE_ERROR((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" SendAdvertisementMessage failed!\n")));

                return;
            }
        }
    }
}

// Helper method to send advertisement message
int iDDSDevice::SendAdvertisementMessage()
{
    try
    {
        if(!AboutNode_writer)
        {
            ACE_ERROR((LM_ERROR,
                              ACE_TEXT("ERROR: AboutNode_writer is null!\n")));

            return -1;
        }

        // Write samples
        RealTimeBackbone::AboutNode nodeAdvertisementMessage;
        nodeAdvertisementMessage.realNodeID.manufacturer = manufacturer.c_str();
        nodeAdvertisementMessage.realNodeID.productType = productType.c_str();
        nodeAdvertisementMessage.realNodeID.serialNumber = serialNumber.c_str();
        nodeAdvertisementMessage.buildStandard.hardwareVersion = hwVersion.c_str();
        nodeAdvertisementMessage.buildStandard.softwareVersion = swVersion.c_str();
        nodeAdvertisementMessage.operationalStatus = RealTimeBackbone::OpStatusReady; //To be updated
        nodeAdvertisementMessage.statusReason = ""; //To be updated
        nodeAdvertisementMessage.logicalNodeID = node_id.c_str();
        nodeAdvertisementMessage.domainID = c_nStreamingDomainID;
        nodeAdvertisementMessage.ipAddress = ipAddress.c_str();
        nodeAdvertisementMessage.time.seconds = 0;
        nodeAdvertisementMessage.time.nanoseconds = 0;

        DDS::ReturnCode_t error = AboutNode_writer->write(nodeAdvertisementMessage, DDS::HANDLE_NIL);

        if (error != DDS::RETCODE_OK)
        {
            ACE_ERROR((LM_ERROR,
                       ACE_TEXT("ERROR: %N:%l: main() -")
                           ACE_TEXT(" write returned %d!\n"),
                       error));
        }

        // Wait for samples to be acknowledged
        //DDS::Duration_t timeout = {30, 0};
        //if (AboutNode_writer->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK)
        //{
        //    ACE_ERROR_RETURN((LM_ERROR,
        //                      ACE_TEXT("ERROR: %N:%l: main() -")
        //                          ACE_TEXT(" wait_for_acknowledgments failed!\n")),
        //                     1);
        //}
    }
    catch (const CORBA::Exception &e)
    {
        e._tao_print_exception("Exception caught sending node advertisment message");
        return 1;
    }

    return 0;
}