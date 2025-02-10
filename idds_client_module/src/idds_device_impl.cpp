#include <idds_client_module/idds_device_impl.h>
#include <opendaq/device_info_factory.h>
#include <opendaq/module_manager_ptr.h>
#include <opendaq/device_type_factory.h>
#include <opendaq/device_domain_factory.h>

BEGIN_NAMESPACE_OPENDAQ_IDDS_CLIENT_MODULE

iDDSDeviceImpl::iDDSDeviceImpl(const ContextPtr& context, const ComponentPtr& parent, const StringPtr& localId)
        : GenericDevice<>(context, parent, localId)
{
}

iDDSDeviceImpl::~iDDSDeviceImpl()
{
}

DeviceInfoPtr iDDSDeviceImpl::onGetInfo()
{
    auto deviceInfo = iDDSDeviceImpl::CreateDeviceInfo();
    deviceInfo.freeze();
    return deviceInfo;
}

DeviceInfoPtr iDDSDeviceImpl::CreateDeviceInfo()
{
    auto devInfo = DeviceInfo("daq.idds://");
    devInfo.setName("iDDS Device");
    devInfo.setSerialNumber("idds_serial");
    devInfo.setManufacturer("openDAQ");
    devInfo.setDeviceType(CreateType());

    return devInfo;
}

DeviceTypePtr iDDSDeviceImpl::CreateType()
{
    return DeviceTypeBuilder()
        .setId("OpenDAQiDDS")
        .setName("iDDSClientDevice")
        .setDescription("iDDS device")
        .setConnectionStringPrefix("daq.idds")
        .build();
}

END_NAMESPACE_OPENDAQ_IDDS_CLIENT_MODULE