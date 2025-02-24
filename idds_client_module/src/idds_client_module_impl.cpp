#include <idds_client_module/idds_client_module_impl.h>
#include <idds_client_module/version.h>
#include <coretypes/version_info_factory.h>
#include <coreobjects/callable_info_factory.h>
#include <coreobjects/argument_info_factory.h>
#include <opendaq/device_type_factory.h>
#include <opendaq/address_info_factory.h>
#include <opendaq/device_impl.h>

BEGIN_NAMESPACE_OPENDAQ_IDDS_CLIENT_MODULE

//using namespace daq::idds;

iDDSClientModule::iDDSClientModule(ContextPtr context)
    : Module("OpenDAQiDDSClientModule",
            daq::VersionInfo(IDDS_CL_MODULE_MAJOR_VERSION, IDDS_CL_MODULE_MINOR_VERSION, IDDS_CL_MODULE_PATCH_VERSION),
            std::move(context),
            "OpenDAQiDDSClientModule")
    , iDDSClient(iDDSDeviceBuilder()
                 .setLogicalNodeId("iDDSClientNode")
                 .build())
{
}

DictPtr<IString, IDeviceType> iDDSClientModule::onGetAvailableDeviceTypes()
{
    auto result = Dict<IString, IDeviceType>();

    auto deviceType = createDeviceType();
    result.set(deviceType.getId(), deviceType);

    return result;
}

DevicePtr iDDSClientModule::onCreateDevice(const StringPtr& connectionString,
                                           const ComponentPtr& parent,
                                           const PropertyObjectPtr& /*config*/)
{
    DevicePtr obj(createWithImplementation<IDevice, Device>(context, parent, "iDDSDevice"));

    //Start iDDS device
    iDDSClient.startServer();

    //Send Message Function
    auto sendMessageProp = FunctionProperty(
            "sendMessage", FunctionInfo(ctString, List<IArgumentInfo>(ArgumentInfo("Message", ctString))));
        FunctionPtr sendMessageCallback = Function(
            [this](ListPtr<IBaseObject> args)
            {
                std::string message = args[0];
                std::cout << "Message sent: " << message << std::endl;
                iDDSClient.sendIDDSMessage("iDDSServerNode", message);
                return 0;
            });

    obj.addProperty(sendMessageProp);
    obj.setPropertyValue("sendMessage", sendMessageCallback);

    //GetAvailableDevices Function
    auto getAvailableDevicesProp = FunctionProperty(
            "getAvailableDevices", ProcedureInfo());
        ProcedurePtr getAvailableDevicesCallback = [this](){
                std::cout << "Available iDDS devices: " << std::endl;
                //return iDDSClient.getAvailableIDDSDevices();
            };

    obj.addProperty(getAvailableDevicesProp);
    obj.setPropertyValue("getAvailableDevices", Procedure(
        [this]()
        {
                auto vec = iDDSClient.getAvailableIDDSDevices();
                std::cout << "Available iDDS devices: " << std::endl;
                for (const auto& id : vec)
                {
                    std::cout << " - " << id << std::endl;
                }
                return iDDSClient.getAvailableIDDSDevices();
        }
    ));

    return obj;
}

DeviceTypePtr iDDSClientModule::createDeviceType()
{
    return DeviceTypeBuilder()
        .setId("OpenDAQiDDS")
        .setName("iDDSClientDevice")
        .setDescription("iDDS device")
        .setConnectionStringPrefix("daq.idds")
        .build();
}

END_NAMESPACE_OPENDAQ_IDDS_CLIENT_MODULE
