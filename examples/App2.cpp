#include <idds_wrapper/idds_device_builder.h>
#include <idds_wrapper/idds_device_impl.h>
#include <iostream>

int main()
{
    iDDSDevice device = iDDSDeviceBuilder().setLogicalNodeId("NodeId2").build();
    device.StartServer();

    while (true)
    {
        std::cout << "Press Enter to print received iDDS messages." << std::endl;
        std::cin.get();
        device.PrintReceivedIDDSMessages();
    }

    return 0;
}
