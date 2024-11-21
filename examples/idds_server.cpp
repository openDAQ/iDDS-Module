#include <opendaq/opendaq.h>
#include <iostream>
#include <thread>

using namespace daq;

int main()
{
    const auto instance = Instance();
    const auto device = instance.addDevice("daqref://device0");

    // Output the name of the added device
    std::cout << device.getInfo().getName() << std::endl;

    // Start an OpcUa server
    instance.addServer("OpenDAQiDDS", nullptr);
    //instance.addServer("OpenDAQOPCUA", nullptr);

	return 0;
}