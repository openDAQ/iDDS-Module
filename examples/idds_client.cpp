#include <opendaq/opendaq.h>
#include <iostream>
#include <thread>

using namespace daq;

int main()
{
    const auto instance = Instance();

    const auto availableDevices = instance.getAvailableDevices();

    auto device = instance.addDevice("daq.idds");
    ProcedurePtr getAvailableDevices = device.getPropertyValue("getAvailableDevices");
    FunctionPtr sendMessage = device.getPropertyValue("sendMessage");

    // Wait for user input
    std::cin.get();

    //GetAvailableDevices
    getAvailableDevices();

    // Wait for user input
    std::cin.get();

    //Send iDDS message to server
    ListPtr<IBaseObject> inputArgs = List<IBaseObject>("Hello from openDaq");
    sendMessage(inputArgs);

    // Wait for user input
    std::cin.get();

    return 0;
}