#include <idds_wrapper/iDDSDevice.h>
#include <iostream>

int main() {
    iDDSDevice device("NodeId1");
    device.StartServer();

    std::cout << "Server started. Press Enter to close." << std::endl;
    std::cin.get();

    return 0;
}
