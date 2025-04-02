#pragma once

#include "dds/dds.hpp"
#include "iDDS.hpp"

struct QoSConfig
{
    std::vector<dds::core::policy::DataRepresentationId> dataRepresentation;
    dds::core::Duration latencyBudget;
    dds::core::policy::Reliability reliability;

    QoSConfig(std::vector<dds::core::policy::DataRepresentationId> reprs = {dds::core::policy::DataRepresentationId::XCDR1},
              dds::core::Duration latency = dds::core::Duration(0, 0),
              dds::core::policy::Reliability reliabilityPolicy = dds::core::policy::Reliability(dds::core::policy::ReliabilityKind_def::BEST_EFFORT,
                                                                                                dds::core::Duration::zero()))
        : dataRepresentation(std::move(reprs)), latencyBudget(latency), reliability(reliabilityPolicy) {}

    // Static helper methods to return different default QoS configurations in case its not specified
    static QoSConfig DefaultParameterDataSeriesReaderQoS()
    {
        return QoSConfig(
            {dds::core::policy::DataRepresentationId::XCDR1},
            dds::core::Duration(25, 0),                             // LatencyBudget: 25ms
            dds::core::policy::Reliability(
                dds::core::policy::ReliabilityKind_def::BEST_EFFORT,
                dds::core::Duration::zero()));
    }

    static QoSConfig DefaultParameterDataSeriesWriterQoS() {
        return QoSConfig(
            {dds::core::policy::DataRepresentationId::XCDR1},
            dds::core::Duration(0, 1500000),                        // LatencyBudget: 1.5ms
            dds::core::policy::Reliability(
                dds::core::policy::ReliabilityKind_def::RELIABLE,
                dds::core::Duration(0, 100000000)));                // Max blocking time: 100ms
    }

    static QoSConfig DefaultMessageReaderQoS()
    {
        return QoSConfig(
            {dds::core::policy::DataRepresentationId::XCDR1},
            dds::core::Duration(0, 100000),                         // LatencyBudget: 100us
            dds::core::policy::Reliability(
                dds::core::policy::ReliabilityKind_def::RELIABLE,
                dds::core::Duration(0, 100000000)));                // Max blocking time: 100ms
    }
};

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
