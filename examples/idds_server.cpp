#include <opendaq/opendaq.h>
#include <iostream>
#include <thread>

using namespace daq;

int main()
{
    const auto instance = Instance();

    // Start an idds server
    instance.addServer("OpenDAQiDDS", nullptr);

    // Wait for user input
    std::cout << "Press Enter to close." << std::endl;
    std::cin.get();

	return 0;
}