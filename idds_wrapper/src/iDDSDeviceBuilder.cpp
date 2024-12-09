#include <idds_wrapper/iDDSDeviceBuilder.h>

iDDSDeviceBuilder& iDDSDeviceBuilder::setManufacturer(const std::string& manufacturer)
{
    _manufacturer = manufacturer;
    return *this;
}

iDDSDeviceBuilder& iDDSDeviceBuilder::setProductType(const std::string& productType)
{
    _productType = productType;
    return *this;
}

iDDSDeviceBuilder& iDDSDeviceBuilder::setSerialNumber(const std::string& serialNumber)
{
    _serialNumber = serialNumber;
    return *this;
}

iDDSDeviceBuilder& iDDSDeviceBuilder::setHWVersion(const std::string& hwVersion)
{
    _hwVersion = hwVersion;
    return *this;
}

iDDSDeviceBuilder& iDDSDeviceBuilder::setSWVersion(const std::string& swVersion)
{
    _swVersion = swVersion;
    return *this;
}

iDDSDeviceBuilder& iDDSDeviceBuilder::setIPAddress(const std::string& ipAddress)
{
    _ipAddress = ipAddress;
    return *this;
}

iDDSDeviceBuilder& iDDSDeviceBuilder::setLogicalNodeId(const std::string& logicalNodeID)
{
    _logicalNodeID = logicalNodeID;
    return *this;
}

// Method to build the iDDSDevice
iDDSDevice iDDSDeviceBuilder::build()
{
    return iDDSDevice(_logicalNodeID, _manufacturer, _productType, _serialNumber, _hwVersion, _swVersion, _ipAddress);
}