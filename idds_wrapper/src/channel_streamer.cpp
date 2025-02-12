#include <idds_wrapper/channel_streamer.h>

#include <chrono>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const char stream_topic[] = "ParameterDataSeries";
static const int c_nParameterID_channel1 = 6445;
static const int c_nParameterID_channel2 = 6446;
//--------------------------------------------------------------------------------------------------

ChannelStreamer::ChannelStreamer(dds::domain::DomainParticipant& participant,
                                 const idds_device_info& device_info)
            : m_participant(participant)
            , m_bRunning(false)
            , m_device_info(device_info)
            , m_streamTopic(participant, stream_topic)
            , m_streamSubscriber(participant)
            , m_streamReader(m_streamSubscriber, m_streamTopic)
            , m_streamPublisher(participant)
            , m_streamWriter(m_streamPublisher, m_streamTopic)
            , m_SubsribedChannel(-1)
            , m_bStreamEnabled(false) //Streaming is disabled by default
{
    // Initialize channels
    m_parameterDataSeries["Channel.1"] = c_nParameterID_channel1;
    m_parameterDataSeries["Channel.2"] = c_nParameterID_channel2;
}

ChannelStreamer::~ChannelStreamer()
{
    // Join threads if they are joinable
    if (m_bRunning)
    {
        StopStreamer();
        StopStreamReader();
    }
}

/// Start the channel streamer thread
void ChannelStreamer::StartStreamer()
{
    // Start the server
    if(!m_bRunning)
    {
        m_bRunning = true;
        m_channelStreamerThread = std::thread(&ChannelStreamer::StartStreaming, this);
    }
}

/// Start the channel reader thread
void ChannelStreamer::StartReaderThread()
{
    // Start the server
    m_channelReaderThread = std::thread(&ChannelStreamer::StartStreamReader, this);
    m_bRunningReader = true;
}

/// Stop the channel streamer thread
void ChannelStreamer::StopStreamer()
{
    if(m_bRunning)
    {
        m_bRunning = false;

        if (m_channelStreamerThread.joinable())
        {
            m_channelStreamerThread.join();
        }
    }
}

void ChannelStreamer::StopStreamReader()
{
    if(m_bRunningReader)
    {
        m_bRunningReader = false;
        if (m_channelReaderThread.joinable())
        {
            m_channelReaderThread.join();
        }
    }
}

/// Method that starts the channel streaming
void ChannelStreamer::StartStreaming()
{
    std::cout << "[iDDS_Wrapper] Streaming started" << std::endl;

    while (m_bRunning && m_bStreamEnabled)
    {
        // Stream data
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        SendSample("Channel.1", 1.0);
        SendSample("Channel.2", 2.0);
    }

    std::cout << "[iDDS_Wrapper] Streaming stopped" << std::endl;
}

void ChannelStreamer::StartStreamReader()
{
    std::cout << "[iDDS_Wrapper] Stream reader started" << std::endl;

    while (m_bRunningReader)
    {
        dds::sub::LoanedSamples<ParameterDataSeries> samples;

        /* Try taking samples from the reader. */
        samples = m_streamReader.take();

        /* Are samples read? */
        if (samples.length() > 0)
        {
            dds::sub::LoanedSamples<ParameterDataSeries>::const_iterator sample_iter;
            for (sample_iter = samples.begin(); sample_iter < samples.end(); ++sample_iter)
            {
                /* Get the message and sample information. */
                const ParameterDataSeries& msg = sample_iter->data();
                const dds::sub::SampleInfo& info = sample_iter->info();

                // Check if this sample has valid data.
                if (info.valid())
                {
                    const ParameterID& samplesParameterID = msg.parameterID();

                    // Only process messages from subscribed channels
                    if(samplesParameterID == m_SubsribedChannel)
                    {
                        std::cout << msg.samples().doubleValue() << ",";
                        std::cout << msg.times().timestamp().seconds() << ",";
                        std::cout << msg.times().timestamp().nanoseconds();
                        std::cout << "\t(value,timestamp(s),timestamp(ns))" << std::endl;
                    }
                }
            }
        }
    }

    std::cout << "[iDDS_Wrapper] Stream reader stopped" << std::endl;
}

// Subscribe to a channel
void ChannelStreamer::SubscribeToChannel(const std::string channelName)
{
    // Check if the channel exists
    if(m_parameterDataSeries.find(channelName) == m_parameterDataSeries.end())
    {
        std::cerr << "[iDDS_Wrapper] Error: Subscription failed - channel does not exist." << std::endl;
        return;
    }
    else
        m_SubsribedChannel = m_parameterDataSeries[channelName];
}

// Send sample method
idds_wrapper_errCode ChannelStreamer::SendSample(const std::string channel, const double value)
{
    try
    {
        ParameterDataSeries dataSeries;

        //parameterID
        dataSeries.parameterID(m_parameterDataSeries[channel]);

        //parameterValues
        dataSeries.samples().doubleValue(value);

        // timestamp
        uint32_t seconds_since_epoch = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
        uint32_t nanoseconds_since_epoch = std::chrono::system_clock::now().time_since_epoch() / std::chrono::nanoseconds(1);

        Timestamps timestamps;
        timestamps._d(TimeType::TimeTypeSingle);
        Time time{seconds_since_epoch, nanoseconds_since_epoch};
        timestamps.timestamp(time);
        dataSeries.times(timestamps);

        //qualityFlags
        dataSeries.qualityFlags(0);

        //instanceValue
        dataSeries.instanceValue(0);

        //write message
        m_streamWriter.write(dataSeries);
        return idds_wrapper_errCode::OK;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "[iDDS_Wrapper] Exception: " << e.what() << std::endl;
        return idds_wrapper_errCode::NOK;
    }
}
