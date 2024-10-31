#include <idds_wrapper/iDDSDevice.h>
#include <iostream>

int main() {
    iDDSDevice device("NodeId2");
    device.StartServer();

    while (true) {
        std::cout << "Press Enter to print received iDDS messages." << std::endl;
        std::cin.get();
        device.PrintReceivedIDDSMessages();
    }

    return 0;
}
