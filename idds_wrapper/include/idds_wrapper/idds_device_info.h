#pragma once

#include "dds/dds.hpp"
#include "iDDS.hpp"

struct idds_device_info
{
    std::string logical_node_id;
    std::string manufacturer;
    std::string productType;
    std::string serialNumber;
    std::string hwVersion;
    std::string swVersion;
    std::string ipAddress;
};
