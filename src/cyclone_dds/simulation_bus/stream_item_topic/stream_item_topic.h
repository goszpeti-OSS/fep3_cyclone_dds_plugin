

#pragma once

#include "../eclipse_cyclone_dds_include.h"
#include <stream_types.hpp>
#include "../topic_intf.h"
#include <fep3/components/logging/logger_intf.h>

#define FEP3_QOS_STREAM_TYPE "fep3::stream_type"
#define FEP3_QOS_PARTICIPANT "fep3::participant"
#define FEP3_DEFAULT_QOS_PROFILE "fep3::default_profile"
#define FEP3_LARGEDATA_QOS_PROFILE_NAME "largedata"

// Will be compute from USER_QOS_PROFILE.xml after ticket FEPSDK-2756
#define FEP3_TRANSPORT_LAYER_MAX_MESSAGE_SIZE 64000

class StreamItemTopic :
    public std::enable_shared_from_this<StreamItemTopic>,
    public ITopic
{

public:
    StreamItemTopic(dds::domain::DomainParticipant & participant
        , const std::string & topic_name
        , const fep3::IStreamType& stream_type
        //, dds::core::detail::QosProvider & qos_provider
        , const std::shared_ptr<fep3::ILogger> logger);

    std::string GetTopic() override;

    std::string findQosProfile(const fep3::IStreamType& stream_type);

    std::unique_ptr<fep3::ISimulationBus::IDataReader> createDataReader
        (size_t queue_capacity
        , const std::weak_ptr<fep3::base::SimulationDataAccessCollection<ReaderItemQueue>>& data_access_collection
        ) override;
    std::unique_ptr<fep3::ISimulationBus::IDataWriter> createDataWriter(size_t queue_capacity) override;

    dds::domain::DomainParticipant & getDomainParticipant();
    dds::topic::Topic<fep3::ddstypes::BusData> getSampleTopic();
    dds::topic::Topic<fep3::ddstypes::StreamType> getStreamTypeTopic();
    dds::core::QosProvider getQosProvider();
    std::string getQosProfile();

    bool updateStreamType(const fep3::IStreamType& stream_type);

private:
    bool isBigStreamType(const fep3::IStreamType& stream_type);
    bool containsProfile(const std::string & profile_name);

private:
    dds::domain::DomainParticipant _participant= dds::core::null;
    dds::topic::Topic<fep3::ddstypes::BusData> _sample_topic = dds::core::null;
    dds::topic::Topic<fep3::ddstypes::StreamType> _stream_type_topic = dds::core::null;
    std::string _topic_name;
    std::string _qos_profile;
    fep3::base::StreamType  _stream_type;
    //dds::core::detail::QosProvider &_qos_provider;
    std::shared_ptr<fep3::ILogger> _logger;
};
