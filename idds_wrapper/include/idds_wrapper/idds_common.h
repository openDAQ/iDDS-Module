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

enum class idds_wrapper_errCode {
    OK,
    NOK,
    CALLBACK_NOT_FOUND,
    CALLBACK_NOT_REGISTERED,
    CALLBACK_ALREADY_REGISTERED,
    RESPONSE_TIMEOUT,
    CHANNEL_ALREADY_EXISTS
};

enum class idds_returnCode {
    OK = 0,
    CommandNotSupported,
    ParametersIncorrect,
    ParameterTooLow,
    ParameterTooHigh,
    IncorrectState,
    OperationInProgress,
    AttributeNotFoundOrInvalid,
    AttributeReadOnly,
    AttributeIncorrectDataType,
    NamespaceNotFoundOrInvalid
};
