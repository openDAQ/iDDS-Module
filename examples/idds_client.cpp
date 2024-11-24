#include <opendaq/opendaq.h>
#include <iostream>
#include <thread>

using namespace daq;

int main()
{
    const auto instance = Instance();

    const auto availableDevices = instance.getAvailableDevices();

    auto device = instance.addDevice("daq.idds");
    FunctionPtr sendMessage = device.getPropertyValue("sendMessage");

    ListPtr<IBaseObject> inputArgs = List<IBaseObject>("Hello from openDaq");
    sendMessage(inputArgs);
}