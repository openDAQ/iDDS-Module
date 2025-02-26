#pragma once

#include "dds/dds.hpp"
#include "iDDS.hpp"

struct IddsDeviceInfo
{
    std::string logical_node_id;
    std::string manufacturer;
    std::string productType;
    std::string serialNumber;
    std::string hwVersion;
    std::string swVersion;
    std::string ipAddress;
};

enum class IddsWrapperErrCode {
    OK,
    NOK,
    CALLBACK_NOT_FOUND,
    CALLBACK_NOT_REGISTERED,
    CALLBACK_ALREADY_REGISTERED,
    RESPONSE_TIMEOUT,
    CHANNEL_ALREADY_EXISTS,
    INVALID_XML_COMMAND
};

enum class IddsReturnCode {
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
