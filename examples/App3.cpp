#include <idds_wrapper/iDDSDeviceBuilder.h>
#include <idds_wrapper/iDDSDevice.h>
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

    device.SendIDDSMessage("NodeId2", "Hello");
    device.SendIDDSMessage("NodeId2", "World");
    device.SendIDDSMessage("NodeId2", "<methodCall> <methodName>General.HardReset</methodName> <params /> </methodCall>");

    std::cout << "Messages sent. Press Enter to close." << std::endl;
    std::cin.get();

    return 0;
}
