#include <idds_wrapper/channel_streamer.h>
#include <idds_wrapper/idds_state_machine.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <chrono>
#include <regex>

//--------------------------------------------------------------------------------------------------
// Constants.
//--------------------------------------------------------------------------------------------------
static const char stream_topic[] = "ParameterDataSeries";
static const char header[]       =  "{{LOGICAL-ID}{SECTION}{NAME}{VALUE}{R/W}{DESCRIPTION}}";
static const int c_nParameterID_channel1 = 6445;
static const int c_nParameterID_channel2 = 6446;
//--------------------------------------------------------------------------------------------------

dds::sub::qos::DataReaderQos getParameterDataSeriesReaderQoSFlags(dds::topic::Topic<ParameterDataSeries>& topic)
{
    dds::sub::qos::DataReaderQos currQos = topic.qos();

    std::vector<dds::core::policy::DataRepresentationId> reprs;
    reprs.push_back(dds::core::policy::DataRepresentationId::XCDR1);
    currQos << dds::core::policy::DataRepresentation(reprs);

    currQos << dds::core::policy::LatencyBudget(dds::core::Duration(25, 0));

    return currQos;
}

dds::pub::qos::DataWriterQos getParameterDataSeriesWriterQoSFlags(dds::topic::Topic<ParameterDataSeries>& topic)
{
    dds::pub::qos::DataWriterQos currQos = topic.qos();

    // Policy.DataRepresentation(use_cdrv0_representation=True, use_xcdrv2_representation=False)
    std::vector<dds::core::policy::DataRepresentationId> reprs;
    reprs.push_back(dds::core::policy::DataRepresentationId::XCDR1);
    currQos << dds::core::policy::DataRepresentation(reprs);

    // Policy.Reliability.Reliable(max_blocking_time=100000000)
    currQos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind_def::RELIABLE, dds::core::Duration(0, 100000000));

    // Policy.LatencyBudget(budget=2500000)
    currQos << dds::core::policy::LatencyBudget(dds::core::Duration(0, 1500000));

    return currQos;
}

ChannelStreamer::ChannelStreamer(dds::domain::DomainParticipant& participant,
                                 const idds_device_info& device_info,
                                 const dds::pub::Publisher& publisher,
                                 const dds::sub::Subscriber& subscriber)
            : m_participant(participant)
            , m_bRunning(false)
            , m_device_info(device_info)
            , m_streamTopic(m_participant, stream_topic)
            , m_streamSubscriber(subscriber)
            , m_streamReader(m_streamSubscriber, m_streamTopic, getParameterDataSeriesReaderQoSFlags(m_streamTopic))
            , m_streamPublisher(publisher)
            , m_streamWriter(m_streamPublisher, m_streamTopic, getParameterDataSeriesWriterQoSFlags(m_streamTopic))
            , m_SubsribedChannel(-1)
            , m_bStreamEnabled(true) //Streaming is enabled by default for now
{
    // Initialize channels
    m_mapSignalIds["Channel.1"] = c_nParameterID_channel1;
    m_mapSignalIds["Channel.2"] = c_nParameterID_channel2;
}

ChannelStreamer::~ChannelStreamer()
{
    // Join threads if they are joinable
    if (m_bRunning)
    {
        stopStreamer();
        stopStreamReader();
    }
}

/// Start the channel streamer thread
void ChannelStreamer::startStreamer()
{
    // Start the server
    if(!m_bRunning)
    {
        m_bRunning = true;
        m_channelStreamerThread = std::thread(&ChannelStreamer::startStreaming, this);
    }
}

/// Start the channel reader thread
void ChannelStreamer::startReaderThread()
{
    // Start the server
    m_channelReaderThread = std::thread(&ChannelStreamer::startStreamReader, this);
    m_bRunningReader = true;
}

/// Stop the channel streamer thread
void ChannelStreamer::stopStreamer()
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

void ChannelStreamer::stopStreamReader()
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
void ChannelStreamer::startStreaming()
{
    std::cout << "[iDDS_Wrapper] Streaming started" << std::endl;

    const double frequency = 0.5;                           // Hz
    const double amplitude = 1.0;                           // Amplitude
    auto start_time = std::chrono::steady_clock::now();     // Time reference

    while (m_bRunning)
    {
        if(IDDSStateMachine::getInstance().getState()  == OperationalStatus::OpStatusOperating)
        {
            // sine wave
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = now - start_time;
            double t = elapsed.count();
            double sine_value = amplitude * std::sin(2.0 * M_PI * frequency * t);
            double sawtooth_value = amplitude * (t * frequency - std::floor(t * frequency));

            // Stream data
            sendSample("Channel.1", sine_value);
            sendSample("Channel.2", sawtooth_value);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::cout << "[iDDS_Wrapper] Streaming stopped" << std::endl;
}

void ChannelStreamer::startStreamReader()
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
void ChannelStreamer::subscribeToChannel(const std::string channelName)
{
    // Check if the channel exists
    if(m_mapSignalIds.find(channelName) == m_mapSignalIds.end())
    {
        std::cerr << "[iDDS_Wrapper] Error: Subscription failed - channel does not exist." << std::endl;
        return;
    }
    else
        m_SubsribedChannel = m_mapSignalIds[channelName];
}

// Subscribe to a channel
void ChannelStreamer::subscribeToChannel(const int channelParamID)
{
    m_SubsribedChannel = channelParamID;
}

// Get channel information
std::string ChannelStreamer::getChannelInfo()
{
    /*
    {{LOGICAL-ID}{SECTION}{NAME}{VALUE}{R/W}{DESCRIPTION}}
    {{OpenDaqDevice1}{Channel.1}{Name}{ChannelName1}{Name of Channel 1}}
    {{OpenDaqDevice1}{Channel.1}{ID}{6445}{ID of Channel 1}}
    {{OpenDaqDevice1}{Channel.2}{Name}{ChannelName2}{Name of Channel 2}}
    {{OpenDaqDevice1}{Channel.2}{ID}{6446}{ID of Channel 2}}
    ...
    */

    std::string strChannelInfo = header;
    std::string aux;
    int channelCount = 1;

    for (auto channel : m_mapSignalIds)
    {
        aux = "{{REPLACELOGICALID}{Channel.REPLACECHANNEL}{Name}{REPLACENAME}{Name of ChannelREPLACECHANNEL}}";
        aux = std::regex_replace(aux, std::regex("REPLACELOGICALID"), m_device_info.logical_node_id);
        aux = std::regex_replace(aux, std::regex("REPLACECHANNEL"), std::to_string(channelCount));
        aux = std::regex_replace(aux, std::regex("REPLACENAME"), channel.first);
        strChannelInfo += aux;

        aux = "{{REPLACELOGICALID}{Channel.REPLACECHANNEL}{ID}{REPLACEID}{ID of Channel ChannelREPLACECHANNEL}}";
        aux = std::regex_replace(aux, std::regex("REPLACELOGICALID"), m_device_info.logical_node_id);
        aux = std::regex_replace(aux, std::regex("REPLACECHANNEL"), std::to_string(channelCount));
        aux = std::regex_replace(aux, std::regex("REPLACEID"), std::to_string(channel.second));
        strChannelInfo += aux;
        channelCount++;
    }

    return strChannelInfo;
}

//Add discovered channels
idds_wrapper_errCode ChannelStreamer::addDiscoverableChannel(const std::string channelName, const int channelParamID)
{
    // Check if the channel exists
    if(m_mapDiscoverableSignalIDs.find(channelName) != m_mapDiscoverableSignalIDs.end())
    {
        std::cerr << "[iDDS_Wrapper] Error: Channel already exists." << std::endl;
        return idds_wrapper_errCode::CHANNEL_ALREADY_EXISTS;
    }

    m_mapDiscoverableSignalIDs[channelName] = channelParamID;
    return idds_wrapper_errCode::OK;
}

// Send sample method
idds_wrapper_errCode ChannelStreamer::sendSample(const std::string channel, const double value)
{
    try
    {
        ParameterDataSeries dataSeries;

        //parameterID
        dataSeries.parameterID(m_mapSignalIds[channel]);

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
