/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>
#include <idds_wrapper/CommandListenerImpl.h>
#include <iostream>

void CommandListenerImpl::on_requested_deadline_missed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedDeadlineMissedStatus & /*status*/)
{
}

void CommandListenerImpl::on_requested_incompatible_qos(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedIncompatibleQosStatus & /*status*/)
{
}

void CommandListenerImpl::on_sample_rejected(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleRejectedStatus & /*status*/)
{
}

void CommandListenerImpl::on_liveliness_changed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::LivelinessChangedStatus & /*status*/)
{
}

void CommandListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
   RealTimeBackbone::MessageDataReader_var reader_i =
      RealTimeBackbone::MessageDataReader::_narrow(reader);

  if (!reader_i)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                   ACE_TEXT(" _narrow failed!\n")));
    ACE_OS::exit(1);
  }

  RealTimeBackbone::Message message;
  DDS::SampleInfo info;

  const DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);

  if (error == DDS::RETCODE_OK)
  {
    if (info.valid_data)
    {
        // Add the received message to the message_vector
        message_vector.push_back(message);

        // Print the received message
        std::cout << "From: = " << message.sourceLogicalNodeID;
        std::cout << "; to: = " << message.targetLogicalNodeID;

        // Printing the message
        std::string strMessageBody(message.messageBody);

        if (strMessageBody.size() > 0)
        {
            std::cout << "; message: = " << strMessageBody << std::endl;
        }
        else
        {
            std::cout << "; message: = [empty]" << std::endl;
        }
    }
  }
  else
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                   ACE_TEXT(" take_next_sample failed!\n")));
  }
}

void CommandListenerImpl::on_subscription_matched(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SubscriptionMatchedStatus & /*status*/)
{
}

void CommandListenerImpl::on_sample_lost(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleLostStatus & /*status*/)
{
}

std::vector<RealTimeBackbone::Message> CommandListenerImpl::get_message_vector()
{
  return message_vector;
}