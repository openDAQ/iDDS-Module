#include <idds_wrapper/iDDSDevice.h>
#include <iostream>
#include <fstream>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const int hello_interval = 1; // seconds
static const char node_advertiser_topic[] = "AboutNode";
static const char message_topic[] = "Message";
static const char rtps_file[] = "rtps.ini";

// Constructor
iDDSDevice::iDDSDevice() : node_id("defaultNode"),
                           listenerNodeAdvertisement_impl(new DataReaderListenerImpl),
                           listenerNodeAdvertisement(listenerNodeAdvertisement_impl),
                           listenerCommand_impl(new CommandListenerImpl),
                           listenerCommand(listenerCommand_impl)
{
    SetupiDDSDevice();
}

iDDSDevice::iDDSDevice(const iDDSNodeUniqueID node_id) : node_id(node_id),
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
    participant->delete_contained_entities();
    dpf->delete_participant(participant);
    TheServiceParticipant->shutdown();
}

/// Initialize Domain Factory and Participant
int iDDSDevice::SetupiDDSDevice()
{
    // Check RTPS ini file exists
    std::ifstream file(rtps_file);
    if (!file.good())
    {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l:")
                              ACE_TEXT("RTPS configuration file was not found!\n")),
                         1);
        return 0;
    }

    // Initialize DomainParticipantFactory
    TheServiceParticipant->default_configuration_file(ACE_TEXT(rtps_file));
    DDS::DomainParticipantFactory_var dpf = TheServiceParticipant->get_domain_participant_factory();

    // Create DomainParticipant
    participant =
        dpf->create_participant(42,
                                PARTICIPANT_QOS_DEFAULT,
                                0,
                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (!participant)
    {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                              ACE_TEXT(" create_participant failed!\n")),
                         1);
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
        // Register TypeSupport (Messenger::iDDSHelloMsg)
        Messenger::iDDSHelloMsgTypeSupport_var ts =
            new Messenger::iDDSHelloMsgTypeSupportImpl;

        if (ts->register_type(participant, "") != DDS::RETCODE_OK)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
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
                              ACE_TEXT("ERROR: %N:%l: main() -")
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
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_publisher failed!\n")),
                             1);
        }

        NodeAdvertiserWriter =
            NodeAdvertiserPublisher->create_datawriter(NodeAdvertiserTopic,
                                         DATAWRITER_QOS_DEFAULT,
                                         0,
                                         OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!NodeAdvertiserWriter)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_datawriter failed!\n")),
                             1);
        }

        iDDSHelloMsg_writer =
            Messenger::iDDSHelloMsgDataWriter::_narrow(NodeAdvertiserWriter);

        if (!iDDSHelloMsg_writer)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
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
                          ACE_TEXT("ERROR: %N:%l: main() -")
                              ACE_TEXT(" create_subscriber failed!\n")),
                         1);
        }

        DDS::DataReaderQos reader_qos;
        NodeAdvertiserSubscriber->get_default_datareader_qos(reader_qos);
        reader_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

        NodeAdvertiserReader =
            NodeAdvertiserSubscriber->create_datareader(NodeAdvertiserTopic,
                                      reader_qos,
                                      listenerNodeAdvertisement,
                                      OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!NodeAdvertiserReader)
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
        e._tao_print_exception("Exception caught in main():");
        return 1;
    }
}

// Create Message Topic
int iDDSDevice::CreateMessageTopic()
{
    try
    {
        // Register TypeSupport (Messenger::iDDSControlMsg)
        Messenger::iDDSControlMsgTypeSupport_var ts =
            new Messenger::iDDSControlMsgTypeSupportImpl;

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
                                                 0,
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
                                                         0,
                                                         OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!MessagePublisher)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_publisher failed!\n")),
                             1);
        }

        MessageWriter = MessagePublisher->create_datawriter(MessageTopic,
                                                            DATAWRITER_QOS_DEFAULT,
                                                            0,
                                                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!MessageWriter)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_datawriter failed!\n")),
                             1);
        }

        iDDSMessage_writer = Messenger::iDDSControlMsgDataWriter::_narrow(MessageWriter);

        if (!iDDSMessage_writer)
        {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" _narrow failed!\n")),
                             1);
        }

        // Create Subscriber
        MessageSubscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                                           0,
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
        e._tao_print_exception("Exception caught in main():");
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
std::vector<iDDSDevice::iDDSNodeUniqueID> iDDSDevice::GetAvailableIDDSDevices()
{
    std::vector<Messenger::iDDSHelloMsg> vec = listenerNodeAdvertisement_impl->get_message_vector();
    std::vector<iDDSNodeUniqueID> available_devices;

    for (const auto &msg : vec)
    {
        available_devices.push_back(msg.unique_id.in());
    }

    // Placeholder implementation
    return available_devices;
}

// SendIDDSMessage method
int iDDSDevice::SendIDDSMessage(const iDDSNodeUniqueID destination_node_id, const std::string message_)
{
    try
    {
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
        Messenger::iDDSControlMsg message;
        message.timestamp = 0;
        message.tid = 0;
        message.from = node_id.c_str();
        message.to = destination_node_id.c_str();

        // The string you want to insert into the command sequence
        const char *command_string = message_.c_str();

        // Determine the length of the string
        size_t string_length = std::strlen(command_string);

        // Make sure the string length does not exceed the max length
        if (string_length > Messenger::IDDS_COMMAND_MAX_LENGTH)
        {
            // Handle the error: string is too long to fit into the iDDSCommand
            std::cerr << "Error: Command string is too long!" << std::endl;
            return 1;
        }

        // Create and set the length of the iDDSCommand sequence
        Messenger::iDDSCommand command;
        command.length(static_cast<CORBA::ULong>(string_length));

        // Copy the string into the command sequence
        std::memcpy(command.get_buffer(), command_string, string_length);

        // Assign the sequence to the command field of the control message
        message.command = command;

        DDS::ReturnCode_t error = iDDSMessage_writer->write(message, DDS::HANDLE_NIL);

        if (error != DDS::RETCODE_OK)
        {
            ACE_ERROR((LM_ERROR,
                       ACE_TEXT("ERROR: %N:%l: main() -")
                           ACE_TEXT(" write returned %d!\n"),
                       error));
        }

        // Wait for samples to be acknowledged
        DDS::Duration_t timeout = {30, 0};
        if (iDDSMessage_writer->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK)
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
        e._tao_print_exception("Exception caught in main():");
        return 1;
    }
}

// PrintReceivedIDDSMessages method
void iDDSDevice::PrintReceivedIDDSMessages()
{
    // Placeholder implementation
    int i = 0;

    std::vector<Messenger::iDDSControlMsg> vec = listenerCommand_impl->get_message_vector();

    for (Messenger::iDDSControlMsg msg : vec)
    {
        std::cout << "#" << i++ << " - ";
        std::cout << "from: = " << msg.from;
        std::cout << "; to: = " << msg.to << std::endl;

        // Printing the command sequence
        if (msg.command.length() > 0) {
            std::string command_str(reinterpret_cast<const char*>(msg.command.get_buffer()), msg.command.length());
            std::cout << "message: = " << command_str << std::endl;
        } else {
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
        std::this_thread::sleep_for(std::chrono::seconds(hello_interval));
        if (m_bRunning)
        {
            SendAdvertisementMessage();
        }
    }
}

// Helper method to send advertisement message
int iDDSDevice::SendAdvertisementMessage()
{
    try
    {
        // Write samples
        Messenger::iDDSHelloMsg message;
        message.timestamp = 0;
        message.unique_id = node_id.c_str();
        message.manufacturer = "OpenDAQ";
        message.model = "model";
        message.serial_number = "serial_number";
        message.logical_id = node_id.c_str();
        message.tags = "tags";

        DDS::ReturnCode_t error = iDDSHelloMsg_writer->write(message, DDS::HANDLE_NIL);

        if (error != DDS::RETCODE_OK)
        {
            ACE_ERROR((LM_ERROR,
                       ACE_TEXT("ERROR: %N:%l: main() -")
                           ACE_TEXT(" write returned %d!\n"),
                       error));
        }

        // Wait for samples to be acknowledged
        //DDS::Duration_t timeout = {30, 0};
        //if (iDDSHelloMsg_writer->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK)
        //{
        //    ACE_ERROR_RETURN((LM_ERROR,
        //                      ACE_TEXT("ERROR: %N:%l: main() -")
        //                          ACE_TEXT(" wait_for_acknowledgments failed!\n")),
        //                     1);
        //}
    }
    catch (const CORBA::Exception &e)
    {
        e._tao_print_exception("Exception caught in main():");
        return 1;
    }

    return 0;
}