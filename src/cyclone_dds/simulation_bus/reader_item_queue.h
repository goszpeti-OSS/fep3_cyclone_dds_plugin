

#pragma once

#include <fep3/base/sample/data_sample_intf.h>
#include <fep3/base/stream_type/stream_type_intf.h>
#include <fep3/components/simulation_bus/simulation_bus_intf.h>
#include <fep3/components/logging/logger_intf.h>

#include <dds/dds.hpp>
#include <dds/core/BuiltinTopicTypes.hpp>
#include <stream_types.hpp>

std::shared_ptr<fep3::arya::IStreamType> createStreamType(const dds::sub::SampleRef<fep3::ddstypes::StreamType>& dds_streamtype, const dds::sub::SampleInfo& sample_info);
std::shared_ptr<fep3::arya::IDataSample> createSample(const dds::sub::SampleRef<fep3::ddstypes::BusData>& dds_sample, const dds::sub::SampleInfo& sample_info);

class StreamItemTopic;

class ReaderItemQueue final
    : public dds::sub::DataReaderListener<fep3::ddstypes::BusData>
{
public:
    ReaderItemQueue
    (const std::shared_ptr<fep3::ILogger>& logger
        , const std::shared_ptr<StreamItemTopic>& topic
        );
    ~ReaderItemQueue();
    size_t size() const;
    size_t capacity() const;
    bool pop(fep3::ISimulationBus::IDataReceiver& receiver);
    
    void setRecreateWaitSetCondition(const std::function<void()> & release_reader_conditions);

    fep3::Optional<fep3::Timestamp> getFrontTime() const;

    void createReader(const dds::sub::qos::DataReaderQos & qos);
    dds::core::cond::Condition createSampleReadCondition(const std::shared_ptr<fep3::ISimulationBus::IDataReceiver>& receiver);
    dds::core::cond::Condition createStreamTypeReadCondition(const std::shared_ptr<fep3::ISimulationBus::IDataReceiver>& receiver);

protected:
    void on_data_available(
        dds::sub::DataReader<fep3::ddstypes::BusData>& reader);

    void on_requested_deadline_missed(
        dds::sub::DataReader<fep3::ddstypes::BusData>& reader,
        const dds::core::status::RequestedDeadlineMissedStatus& status);

    void on_requested_incompatible_qos(
        dds::sub::DataReader<fep3::ddstypes::BusData>& reader,
        const dds::core::status::RequestedIncompatibleQosStatus& status);

    void on_sample_rejected(
        dds::sub::DataReader<fep3::ddstypes::BusData>& reader,
        const dds::core::status::SampleRejectedStatus& status);

    void on_liveliness_changed(
        dds::sub::DataReader<fep3::ddstypes::BusData>& reader,
        const dds::core::status::LivelinessChangedStatus& status);

    void on_subscription_matched(
        dds::sub::DataReader<fep3::ddstypes::BusData>& reader,
        const dds::core::status::SubscriptionMatchedStatus& status);

    void on_sample_lost(
        dds::sub::DataReader<fep3::ddstypes::BusData>& reader,
        const dds::core::status::SampleLostStatus& status);

private:
    bool popFrom
        (dds::sub::DataReader<fep3::ddstypes::BusData>& sample_reader
        , dds::sub::DataReader<fep3::ddstypes::StreamType>& streamtype_reader
        , const dds::sub::Subscriber& subscriber
        , fep3::ISimulationBus::IDataReceiver& receiver
        );
    void logError(const fep3::Result& res) const;

private:
    std::shared_ptr<fep3::ILogger> _logger;
    std::shared_ptr<StreamItemTopic> _topic;
    dds::sub::Subscriber _subscriber = dds::core::null;

    std::function<void()> _release_reader_conditions;
    dds::sub::DataReader<fep3::ddstypes::BusData> _sample_reader = dds::core::null;
    dds::sub::DataReader<fep3::ddstypes::StreamType> _streamtype_reader = dds::core::null;
};
