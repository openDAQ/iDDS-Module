#include <idds_wrapper/iDDSDevice.h>
#include <iDDSTypeSupportImpl.h>
#include <iDDSC.h>

class iDDSDeviceBuilder
{
public:
    iDDSDeviceBuilder& setManufacturer(const std::string& manufacturer);

    iDDSDeviceBuilder& setProductType(const std::string& productType);

    iDDSDeviceBuilder& setSerialNumber(const std::string& serialNumber);

    iDDSDeviceBuilder& setHWVersion(const std::string& hwVersion);

    iDDSDeviceBuilder& setSWVersion(const std::string& swVersion);

    iDDSDeviceBuilder& setIPAddress(const std::string& ipAddress);

    iDDSDeviceBuilder& setLogicalNodeId(const std::string& logicalNodeID);

    iDDSDevice build();

private:
    std::string _manufacturer;
    std::string _productType;
    std::string _serialNumber;
    std::string _hwVersion;
    std::string _swVersion;
    std::string _ipAddress;
    std::string _logicalNodeID;
};