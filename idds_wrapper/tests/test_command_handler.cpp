#include "gtest/gtest.h"
#include "idds_wrapper/command_handler.h"
#include "gmock/gmock.h"
#include "idds_wrapper/idds_common.h"
#include "idds_wrapper/idds_state_machine.h"
#include "idds_wrapper/channel_streamer.h"
#include "dds/dds.hpp"
#include "iDDS.hpp"
#include <string>
#include <vector>
#include <map>

using namespace testing;

class MockChannelStreamer : public ChannelStreamer
{
public:
    MockChannelStreamer(dds::domain::DomainParticipant& participant, const IddsDeviceInfo& device_info,
                        dds::pub::Publisher& publisher, dds::sub::Subscriber& subscriber, const QoSConfig& readerQoSConfig, const QoSConfig& writerQoSConfig)
        : ChannelStreamer(participant, device_info, publisher, subscriber, readerQoSConfig, writerQoSConfig) {}

    MOCK_METHOD(std::string, getChannelInfo, (), (override));
};

class CommandHandlerTest : public ::testing::Test {
protected:
    dds::domain::DomainParticipant participant;
    IddsDeviceInfo device_info;
    dds::sub::Subscriber subscriber;
    dds::pub::Publisher publisher;

    MockChannelStreamer channelStreamer;
    CommandHandler* commandHandler;

    CommandHandlerTest()
        : participant(2),
          device_info{"node_id", "manufacturer", "productType", "serialNumber", "hwVersion", "swVersion", "ipAddress"},
          subscriber(dds::sub::Subscriber(participant, participant.default_subscriber_qos() << dds::core::policy::Partition("default"))),
          publisher(dds::pub::Publisher(participant, participant.default_publisher_qos() << dds::core::policy::Partition("default"))),
          channelStreamer(participant, device_info, publisher, subscriber, QoSConfig::DefaultParameterDataSeriesReaderQoS(), QoSConfig::DefaultParameterDataSeriesWriterQoS())
    {
    }

    void SetUp() override {
        commandHandler = new CommandHandler(participant, device_info, channelStreamer, QoSConfig::DefaultMessageReaderQoS());
    }

    void TearDown() override {
        delete commandHandler;
    }
};

TEST_F(CommandHandlerTest, CommandHandler_SendIDDSMessage)
{
    std::string destination_node_id = "destinationNode";
    std::string message = "TestMessage";
    IddsWrapperErrCode result = commandHandler->sendIDDSMessage(destination_node_id, message);

    EXPECT_EQ(static_cast<int>(result), static_cast<int>(IddsWrapperErrCode::OK));
}

TEST_F(CommandHandlerTest, PublishCommandAndWaitForReply_ValidCommand)
{
    // Start MessageReader
    commandHandler->start();

    std::string destination_node_id = "node_id";
    std::string message = "<methodCall><methodName>General.StartOperating</methodName></methodCall>";
    IddsWrapperErrCode result = commandHandler->publishCommandAndWaitForReply(destination_node_id, message);

    EXPECT_EQ(static_cast<int>(result), static_cast<int>(IddsWrapperErrCode::OK));

    // Stop MessageReader
    commandHandler->stop();
}

TEST_F(CommandHandlerTest, PublishCommandAndWaitForReply_GetAttribute)
{
    // Start MessageReader
    commandHandler->start();

    std::string destination_node_id = "node_id";
    std::string message = "<methodCall><methodName>Configuration.GetAttribute</methodName><params><param><name>Attribute</name>" \
                          "<value><String>alistv</String></value></param></params></methodCall>";

    EXPECT_CALL(channelStreamer, getChannelInfo()).WillOnce(Return("{{LOGICAL-ID}{SECTION}{NAME}{VALUE}{R/W}{DESCRIPTION}}"                 \
                                                                    "{{OpenDaqDevice1}{Channel.1}{Name}{ChannelName1}{Name of Channel 1}}"  \
                                                                    "{{OpenDaqDevice1}{Channel.1}{ID}{6445}{ID of Channel 1}}"));

    IddsWrapperErrCode result = commandHandler->publishCommandAndWaitForReply(destination_node_id, message);

    //Print received messages
    auto receivedMessages = commandHandler->getReceivedIDDSMessages();

    ASSERT_EQ(static_cast<int>(result), static_cast<int>(IddsWrapperErrCode::OK));
    EXPECT_EQ(receivedMessages[1].messageBody(), "<methodResponse><code>0</code><message /><params><param><name>Attribute</name>" \
                        "<value><String>{{LOGICAL-ID}{SECTION}{NAME}{VALUE}{R/W}{DESCRIPTION}}"                                   \
                        "{{OpenDaqDevice1}{Channel.1}{Name}{ChannelName1}{Name of Channel 1}}"                                    \
                        "{{OpenDaqDevice1}{Channel.1}{ID}{6445}{ID of Channel 1}}</String></value></param></params></methodResponse>");

    // Stop MessageReader
    commandHandler->stop();
}

TEST_F(CommandHandlerTest, PublishCommandAndWaitForReply_InvalidCommand)
{
    // Start MessageReader
    commandHandler->start();

    std::string destination_node_id = "node_id";
    std::string message = "Invalid";
    IddsWrapperErrCode result = commandHandler->publishCommandAndWaitForReply(destination_node_id, message);

    EXPECT_EQ(static_cast<int>(result), static_cast<int>(IddsWrapperErrCode::RESPONSE_TIMEOUT));

    // Stop MessageReader
    commandHandler->stop();
}

TEST_F(CommandHandlerTest, PublishCommandAndWaitForReply_NoReply)
{
    // Start MessageReader
    commandHandler->start();

    std::string destination_node_id = "WrongID";
    std::string message = "<methodCall><methodName>General.StartOperating</methodName></methodCall>";
    IddsWrapperErrCode result = commandHandler->publishCommandAndWaitForReply(destination_node_id, message);

    EXPECT_EQ(static_cast<int>(result), static_cast<int>(IddsWrapperErrCode::RESPONSE_TIMEOUT));

    // Stop MessageReader
    commandHandler->stop();
}
