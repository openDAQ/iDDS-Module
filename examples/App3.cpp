#include <idds_wrapper/idds_device_builder.h>
#include <idds_wrapper/idds_device_impl.h>
#include <iostream>

int main()
{
    iDDSDevice device = iDDSDeviceBuilder().setLogicalNodeId("NodeId3").build();
    device.startServer();

    std::cout << "Press Enter to check available iDDS devices." << std::endl;
    std::cin.get();

    std::cout << "Available iDDS devices: " << std::endl;
    auto devices = device.getAvailableIDDSDevices();
    for (const auto& id : devices)
    {
        std::cout << " - " << id << std::endl;
    }

    std::cout << "Publish Command: Configuration.GetAttribute" << std::endl;
    std::cin.get();

    // Configuration.GetAttribute
    device.publishCommand(devices[0], "Configuration.GetAttribute");

    std::cin.get();

    // Get available signals name and ID
    auto map = device.getDiscoverableSignalNameAndID();

    for(const auto& [name, id] : map)
    {
        std::cout << "Signal name: " << name << " ID: " << id << std::endl;
    }

    std::cin.get();

    // Subscribe channel
    device.subscribeToChannel(6445);
    std::cout << "Subscribed to channel ID:6445" << std::endl;

    std::cin.get();

    // General.StartOperating
    std::cout << "Publish Command: General.StartOperating" << std::endl;
    device.publishCommand(devices[0], "General.StartOperating");


    std::cin.get();

    // General.StopOperating
    std::cout << "Publish Command: General.StopOperating" << std::endl;
    device.publishCommand(devices[0], "General.StopOperating");

    std::cin.get();

    std::cout << "Subscribed to channel ID:6446" << std::endl;
    device.subscribeToChannel(6446);

    std::cin.get();

    // General.StartOperating
    std::cout << "Publish Command: General.StartOperating" << std::endl;
    device.publishCommand(devices[0], "General.StartOperating");


    std::cin.get();

    // General.StopOperating
    std::cout << "Publish Command: General.StopOperating" << std::endl;
    device.publishCommand(devices[0], "General.StopOperating");

    return 0;
}
