#include <idds_wrapper/idds_device_builder.h>
#include <idds_wrapper/idds_device_impl.h>
#include <iostream>

int main()
{
    iDDSDevice device = iDDSDeviceBuilder().setLogicalNodeId("NodeId1").build();

    device.StartServer();

    std::cout << "Server started. Press Enter to close." << std::endl;
    std::cin.get();

    auto commands = device.getSupportedIDDSCommands();

    for (auto command : commands)
    {
        std::cout << command << std::endl;
    }

    std::cin.get();
    return 0;
}
