#pragma once
#include <thread>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

#include "dds/dds.hpp"
#include "iDDS.hpp"

#include <idds_wrapper/idds_common.h>

class ChannelStreamer
{
public:
    ChannelStreamer(dds::domain::DomainParticipant& participant,
                    const idds_device_info& device_info);
    ~ChannelStreamer();

    /// Start the channel streamer thread
    void StartStreamer();

    /// Start the channel reader thread
    void StartReaderThread();

    /// Stop the channel streamer thread
    void StopStreamer();

    /// Stop the channel reader thread
    void StopStreamReader();

    /// Method that starts the channel streaming
    void StartStreaming();

    /// Method that prints out samples from subscribed channel
    void StartStreamReader();

    /// Subscribe to a channel
    void SubscribeToChannel(const std::string channelName);

    /// Subscribe to a channel
    void SubscribeToChannel(const int channelParamID);

    // Get channel information
    std::string getChannelInfo();
private:
    /// Send sample
    idds_wrapper_errCode SendSample(const std::string channel, const double value);

private:
    std::thread m_channelStreamerThread;
    std::thread m_channelReaderThread;

    ParameterID m_SubsribedChannel;

    bool m_bRunning;
    bool m_bStreamEnabled;
    bool m_bRunningReader;

    idds_device_info m_device_info;

    dds::domain::DomainParticipant& m_participant;
    //ParameterDataSeries Topic
    dds::topic::Topic<ParameterDataSeries>      m_streamTopic;
    dds::sub::Subscriber                        m_streamSubscriber;
    dds::sub::DataReader<ParameterDataSeries>   m_streamReader;
    dds::pub::Publisher                         m_streamPublisher;
    dds::pub::DataWriter<ParameterDataSeries>   m_streamWriter;

    std::unordered_map<std::string, ParameterID> m_parameterDataSeries;
};