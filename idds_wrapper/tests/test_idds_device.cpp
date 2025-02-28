#include "gtest/gtest.h"

#include "idds_wrapper/idds_common.h"
#include "idds_wrapper/idds_device_builder.h"
#include "idds_wrapper/idds_device_impl.h"
#include "idds_wrapper/command_processor.h"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>

using namespace testing;

class iDDSDeviceTest : public ::testing::Test
{
protected:
    iDDSDevice *device1, *device2;

    void SetUp() override
    {
        device1 = new iDDSDevice(iDDSDeviceBuilder().setLogicalNodeId("idds_node_1").build());
        device2 = new iDDSDevice(iDDSDeviceBuilder().setLogicalNodeId("idds_node_2").build());
    }

    void TearDown() override
    {
        delete device1;
        delete device2;
    }
};

TEST_F(iDDSDeviceTest, IDDSDevice_GetSupportedIDDSCommands)
{
    std::vector<std::string> commands = device1->getSupportedIDDSCommands();

    EXPECT_EQ(std::find(commands.begin(), commands.end(), "General.StartOperating") != commands.end(), true);
    EXPECT_EQ(std::find(commands.begin(), commands.end(), "General.StopOperating") != commands.end(), true);
    EXPECT_EQ(std::find(commands.begin(), commands.end(), "Configuration.GetAttribute") != commands.end(), true);
}

TEST_F(iDDSDeviceTest, IDDSDevice_Discovery)
{
    // Start Server
    device1->startServer();
    device2->startServer();

    std::this_thread::sleep_for(std::chrono::seconds(6)); //Messages are sent every 5 seconds to the AboutNode

    auto foundDevices = device1->getAvailableIDDSDevices();

    EXPECT_EQ(std::find(foundDevices.begin(), foundDevices.end(), "idds_node_2") != foundDevices.end(), true);

    // Stop Server
    device1->stopServer();
    device2->stopServer();
}

TEST_F(iDDSDeviceTest, IDDSDevice_Start_Stop_Operating)
{
    // Start Server
    device1->startServer();
    device2->startServer();

    // General.StartOperating
    device1->publishCommand("idds_node_2", "General.StartOperating");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_EQ(device2->getCurrentDeviceState(), OperationalStatus::OpStatusOperating);

    // General.StopOperating
    device1->publishCommand("idds_node_2", "General.StopOperating");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_EQ(device2->getCurrentDeviceState(), OperationalStatus::OpStatusReady);

    // Stop Server
    device1->stopServer();
    device2->stopServer();
}
