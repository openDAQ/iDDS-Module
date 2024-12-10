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
   Messenger::iDDSControlMsgDataReader_var reader_i =
      Messenger::iDDSControlMsgDataReader::_narrow(reader);

  if (!reader_i)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                   ACE_TEXT(" _narrow failed!\n")));
    ACE_OS::exit(1);
  }

  Messenger::iDDSControlMsg message;
  DDS::SampleInfo info;

  const DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);

  if (error == DDS::RETCODE_OK)
  {
    if (info.valid_data)
    {
        // Add the received message to the message_vector
        message_vector.push_back(message);

        // Print the received message
        std::cout << "From: = " << message.from;
        std::cout << "; to: = " << message.to;

        // Printing the command sequence
        if (message.command.length() > 0) {
            std::string command_str(reinterpret_cast<const char*>(message.command.get_buffer()), message.command.length());
            std::cout << "; message: = " << command_str << std::endl;
        } else {
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

std::vector<Messenger::iDDSControlMsg> CommandListenerImpl::get_message_vector()
{
  return message_vector;
}