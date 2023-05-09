


#include <fep3/base/stream_type/default_stream_type.h>
#include "stream_item_topic.h"
#include "stream_item_reader.h"
#include "stream_item_writer.h"

using namespace fep3;
using namespace dds::domain;

StreamItemTopic::StreamItemTopic(DomainParticipant & participant
    , const std::string & topic_name
    , const IStreamType& stream_type
    //, dds::core::QosProvider & qos_provider
    , const std::shared_ptr<fep3::ILogger> logger)
     : _participant(participant)
     , _topic_name(topic_name)
     , _stream_type(stream_type)
    //  , _qos_provider(qos_provider)
     , _logger(logger)
     , _stream_type_topic(dds::topic::Topic<fep3::ddstypes::StreamType> (participant, topic_name + "_stream_type"))
     , _sample_topic(dds::topic::Topic<fep3::ddstypes::BusData> (participant, topic_name, 
         dds::topic::qos::TopicQos()
         << dds::core::policy::Reliability::Reliable(dds::core::Duration::from_millisecs(1000))
         << dds::core::policy::History::KeepLast(1)
         << dds::core::policy::ResourceLimits(100)
         ))
{
    _qos_profile = findQosProfile(stream_type);

    if (logger->isDebugEnabled())
    {
        logger->logDebug(a_util::strings::format("Using qos profile '%s' for topic '%s'.", _qos_profile.c_str(), _topic_name.c_str()));
    }
}

 std::string StreamItemTopic::GetTopic()
 {
     return _topic_name;
 }

 bool StreamItemTopic::updateStreamType(const fep3::IStreamType& stream_type)
 {
     if(!(stream_type == _stream_type))
     {
         auto qos_profile = findQosProfile(stream_type);
         if (qos_profile != _qos_profile)
         {
             if (_logger->isDebugEnabled())
             {
                 _logger->logDebug(a_util::strings::format("Update qos profile for topic '%s' from '%s' to '%s'.", _topic_name.c_str(), _qos_profile.c_str(), qos_profile.c_str()));
             }

             _qos_profile = qos_profile;
             return true;
         }
     }
     return false;
 }

 bool StreamItemTopic::isBigStreamType(const fep3::IStreamType& stream_type)
 {
    
     auto max_byte_size = stream_type.getProperty(fep3::base::arya::meta_type_prop_name_max_byte_size);
     if (!max_byte_size.empty()
         && std::stoi(max_byte_size) >= FEP3_TRANSPORT_LAYER_MAX_MESSAGE_SIZE)
     {
         return true;
     }

     // Need to compute size of all posible array types here. See FEPSDK-2934
     // Remove lines bellow just a approximation 
     if (stream_type.getMetaTypeName() == fep3::base::arya::meta_type_plain_array.getName())
     {
         auto max_array_size = stream_type.getProperty(fep3::base::arya::meta_type_prop_name_max_array_size);

         // Assume 8 Bytes for the biggest plain type
         if (!max_array_size.empty()
             && std::stoi(max_array_size) * 8 >= FEP3_TRANSPORT_LAYER_MAX_MESSAGE_SIZE)
         {
             return true;
         }
     }
     return false;
 }

 std::string StreamItemTopic::findQosProfile(const fep3::IStreamType& stream_type)
 {
     auto qos_profile_name = stream_type.getMetaTypeName();

     if (isBigStreamType(stream_type))
     {
         std::string big_qos_profile_name =  qos_profile_name + std::string("_big");

         if (containsProfile(big_qos_profile_name))
         {
             if (_logger->isDebugEnabled())
             {
                 _logger->logDebug(a_util::strings::format("stream_type '%s' for topic '%s' is too big and needs to be fragmented. Using profile '%s'."
                     , qos_profile_name.c_str()
                     , _topic_name.c_str()
                     , big_qos_profile_name.c_str()));
             }

             return std::string("fep3::") + big_qos_profile_name;
         }
     }

     if (containsProfile(qos_profile_name))
     {
         return std::string("fep3::") + qos_profile_name;
     }
     else
     {
         if (_logger->isWarningEnabled())
         {
             _logger->logWarning(a_util::strings::format("MetaType '%s' not defined in USER_QOS_PROFILES.xml. Using '" FEP3_DEFAULT_QOS_PROFILE "'.", qos_profile_name.c_str()));
         }
     }

     return FEP3_DEFAULT_QOS_PROFILE;
 }

 bool StreamItemTopic::containsProfile(const std::string & profile_name)
 {
    // TODO not implemented, can't read from file
     return false;
 }

std::unique_ptr<ISimulationBus::IDataReader> StreamItemTopic::createDataReader
    (size_t queue_capacity
    , const std::weak_ptr<fep3::base::SimulationDataAccessCollection<ReaderItemQueue>>& data_access_collection
    )
{
    return std::make_unique<StreamItemDataReader>
        (this->shared_from_this()
        , queue_capacity
        , data_access_collection
        , _logger);
}

std::unique_ptr<ISimulationBus::IDataWriter> StreamItemTopic::createDataWriter(size_t queue_capacity)
{
    auto writer = std::make_unique<StreamItemDataWriter>(this->shared_from_this(), queue_capacity);// , _qos_provider);
    writer->write(_stream_type);
    return writer;
}

 dds::domain::DomainParticipant & StreamItemTopic::getDomainParticipant()
 {
     return _participant;
 }

 dds::topic::Topic<fep3::ddstypes::BusData> StreamItemTopic::getSampleTopic()
 {
     return _sample_topic;
 }

 dds::topic::Topic<fep3::ddstypes::StreamType> StreamItemTopic::getStreamTypeTopic()
 {
     return _stream_type_topic;
 }

 std::string StreamItemTopic::getQosProfile()
 {
     return _qos_profile;
 }
