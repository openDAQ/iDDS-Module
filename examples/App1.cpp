#include <idds_wrapper/iDDSDeviceBuilder.h>
#include <idds_wrapper/iDDSDevice.h>
#include <iostream>

int main()
{
    iDDSDevice device = iDDSDeviceBuilder().setLogicalNodeId("NodeId1").build();

    device.StartServer();

    std::cout << "Server started. Press Enter to close." << std::endl;
    std::cin.get();

    return 0;
}
