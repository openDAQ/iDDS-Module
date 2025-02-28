#include "gtest/gtest.h"
#include "idds_wrapper/idds_common.h"
#include "idds_wrapper/command_processor.h"

using namespace testing;

class CommandProcessorTest : public ::testing::Test {
protected:
    CommandProcessor commandProcessor;

    void SetUp() override {
        // Register a sample callback for testing
        commandProcessor.registerCallback("TestCommand", [](const ParamList& params, std::string& response) {
            response = "Callback executed";
        });
    }
};

TEST_F(CommandProcessorTest, ProcessCommand_GetRegisteredCommands)
{
    std::vector<std::string> commands = commandProcessor.getSupportedCommands();

    EXPECT_EQ(commands.size(), 1);
    EXPECT_EQ(commands[0], "TestCommand");
}

TEST_F(CommandProcessorTest, ProcessCommand_GetCommandXML)
{
    std::string expectedXML = "<methodCall><methodName>General.StartOperating</methodName><params /></methodCall>";

    std::string commandXML = commandProcessor.getCommandXML("General.StartOperating");
    EXPECT_EQ(expectedXML, commandXML);
}

TEST_F(CommandProcessorTest, ProcessCommand_CallbackFound)
{
    std::string response;
    ParamList params;
    IddsWrapperErrCode result = commandProcessor.processCommand("TestCommand", params, response);

    EXPECT_EQ(static_cast<int>(result), static_cast<int>(IddsWrapperErrCode::OK));
    EXPECT_EQ(response, "Callback executed");
}


TEST_F(CommandProcessorTest, ProcessCommand_CallbackNotFound)
{
    std::string response;
    ParamList params;
    IddsWrapperErrCode result = commandProcessor.processCommand("NonExistentCommand", params, response);

    EXPECT_EQ(static_cast<int>(result), static_cast<int>(IddsWrapperErrCode::CALLBACK_NOT_FOUND));
    EXPECT_EQ(response, "");
}
