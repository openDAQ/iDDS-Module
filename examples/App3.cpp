#include <idds_wrapper/idds_device_builder.h>
#include <idds_wrapper/idds_device_impl.h>
#include <iostream>

int main()
{
    iDDSDevice device = iDDSDeviceBuilder().setLogicalNodeId("NodeId3").build();
    device.StartServer();

    std::cout << "Press Enter to check available iDDS devices." << std::endl;
    std::cin.get();

    std::cout << "Available iDDS devices: " << std::endl;
    auto devices = device.GetAvailableIDDSDevices();
    for (const auto& id : devices)
    {
        std::cout << " - " << id << std::endl;
    }

    std::cout << "Press Enter to send iDDS messages." << std::endl;
    std::cin.get();

    // Configuration.GetAttribute
    device.SendIDDSMessage("NodeId1", "<methodCall> <methodName>Configuration.GetAttribute</methodName> <params> <param> <name>Attribute</name> <value><String>alistv</String></value> </param> </params> </methodCall>");

    std::cin.get();

    // General.StartOperating
    device.SendIDDSMessage("NodeId1", "<methodCall> <methodName>General.StartOperating</methodName> <params /> </methodCall>");

    std::cin.get();

    // Subscrive to channel
    device.SubscribeToChannel(6445);

    std::cin.get();

    // General.StopOperating
    device.SendIDDSMessage("NodeId1", "<methodCall> <methodName>General.StopOperating</methodName> <params /> </methodCall>");

    std::cin.get();

    return 0;
}
