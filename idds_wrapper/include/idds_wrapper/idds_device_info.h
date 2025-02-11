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

struct message_writer_info
{
    dds::topic::Topic<Message>    topic;
    dds::pub::Publisher           publisher;
    dds::pub::DataWriter<Message> writer;

    message_writer_info(const dds::topic::Topic<Message>& t,
                        const dds::pub::Publisher& p,
                        const dds::pub::DataWriter<Message>& w)
        : topic(t), publisher(p), writer(w) {}
};

static const char message_topic_prefix[] = "Message";