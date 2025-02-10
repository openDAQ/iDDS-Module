#pragma once

#include <opendaq/device_impl.h>
#include <idds_client_module/common.h>

BEGIN_NAMESPACE_OPENDAQ_IDDS_CLIENT_MODULE

class iDDSDeviceImpl final : public Device
{
public:
    explicit iDDSDeviceImpl(const ContextPtr& context, const ComponentPtr& parent, const StringPtr& localId);
    ~iDDSDeviceImpl() override;

    static DeviceInfoPtr CreateDeviceInfo();
    static DeviceTypePtr CreateType();

    // IDevice
    DeviceInfoPtr onGetInfo() override;

private:
    std::string serialNumber;
};

END_NAMESPACE_OPENDAQ_IDDS_CLIENT_MODULE