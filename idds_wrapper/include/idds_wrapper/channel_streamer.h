#pragma once
#include <thread>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "dds/dds.hpp"
#include "iDDS.hpp"

#include <idds_wrapper/idds_common.h>

class ChannelStreamer
{
public:
    ChannelStreamer(dds::domain::DomainParticipant& participant,
                    const IddsDeviceInfo& device_info,
                    const dds::pub::Publisher& publisher,
                    const dds::sub::Subscriber& subscriber,
                    const QoSConfig& readerQoSConfig,
                    const QoSConfig& writerQoSConfig);
    ~ChannelStreamer();

    /// Start the channel streamer thread
    void startStreamer();

    /// Start the channel reader thread
    void startReaderThread();

    /// Stop the channel streamer thread
    void stopStreamer();

    /// Stop the channel reader thread
    void stopStreamReader();

    /// Method that starts the channel streaming
    void startStreaming();

    /// Method that prints out samples from subscribed channel
    void startStreamReader();

    /// Subscribe to a channel
    void subscribeToChannel(const std::string channelName);

    /// Subscribe to a channel
    void subscribeToChannel(const int channelParamID);

    // Get channel information
    virtual std::string getChannelInfo();

    //Add discovered channels
    IddsWrapperErrCode addDiscoverableChannel(const std::string& channelName, const int& channelParamID);

    // Get discoverable signal name and ID
    std::map<std::string, ParameterID> getDiscoverableSignalNameAndID() { return m_mapDiscoverableSignalIDs; }
private:
    /// Send sample
    IddsWrapperErrCode sendSample(const std::string& channel, const double& value);

private:
    std::thread m_channelStreamerThread;
    std::thread m_channelReaderThread;

    ParameterID m_SubsribedChannel;

    bool m_bRunning;
    bool m_bStreamEnabled;
    bool m_bRunningReader;

    IddsDeviceInfo m_device_info;

    dds::domain::DomainParticipant& m_participant;
    //ParameterDataSeries Topic
    dds::topic::Topic<ParameterDataSeries>      m_streamTopic;
    dds::sub::Subscriber                        m_streamSubscriber;
    dds::sub::DataReader<ParameterDataSeries>   m_streamReader;
    dds::pub::Publisher                         m_streamPublisher;
    dds::pub::DataWriter<ParameterDataSeries>   m_streamWriter;

    std::map<std::string, ParameterID> m_mapSignalIds;
    std::map<std::string, ParameterID> m_mapDiscoverableSignalIDs;

    uint32_t m_InstanceValue;
};
