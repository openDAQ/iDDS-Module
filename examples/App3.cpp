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
    device.SubscribeToChannel(6445);
    std::cout << "Subscribed to channel" << std::endl;

    std::cin.get();

    // General.StartOperating
    device.publishCommand(devices[0], "General.StartOperating");


    std::cin.get();

    // General.StopOperating
    device.publishCommand(devices[0], "General.StopOperating");

    std::cin.get();

    return 0;
}
