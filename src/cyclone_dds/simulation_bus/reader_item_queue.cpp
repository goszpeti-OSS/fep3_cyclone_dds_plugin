


//include <iostream>

#include <a_util/result.h>

#include <fep3/base/sample/data_sample.h>

#include "converter.h"

#include "reader_item_queue.h"
#include "stream_item_topic/stream_item_topic.h"


std::shared_ptr<fep3::IStreamType> createStreamType(dds::sub::SampleRef<fep3::ddstypes::StreamType>& dds_streamtype, const dds::sub::SampleInfo& /*sample_info*/)
{
    auto streamtype = std::make_shared<fep3::base::StreamType>(dds_streamtype.data().metatype());
    for (auto dds_property : dds_streamtype.data().properties())
    {
        streamtype->setProperty(dds_property.name(), dds_property.value(), dds_property.type());
    }
    return streamtype;
}

std::shared_ptr<fep3::IDataSample> createSample(dds::sub::SampleRef<fep3::ddstypes::BusData>& dds_sample, const dds::sub::SampleInfo& sample_info)
{
    auto sample = std::make_shared<fep3::base::DataSample>();
    auto da1 = dds_sample.data();
    sample->set(da1.data().data(), da1.data().size());
    sample->setTime(convertTimestamp(sample_info.timestamp()));
    sample->setCounter(static_cast<uint32_t>(sample_info.generation_count()->no_writers()));
    return sample;
}

ReaderItemQueue::ReaderItemQueue
    (const std::shared_ptr<fep3::ILogger>& logger
    , const std::shared_ptr<StreamItemTopic> & topic
    ):
    _logger(logger)
    , _topic(topic)
    , _subscriber(topic->getDomainParticipant() , topic->getDomainParticipant().default_subscriber_qos()) // << dds::core::policy::Presentation::TopicAccessScope(false, false))
{
    _streamtype_reader = dds::sub::DataReader<fep3::ddstypes::StreamType>(_subscriber
        , _topic->getStreamTypeTopic(), _subscriber->default_datareader_qos());

    createReader(_subscriber->default_datareader_qos());
}

ReaderItemQueue::~ReaderItemQueue()
{
    _sample_reader->close();
    _streamtype_reader->close();
    _subscriber.close();

}

void ReaderItemQueue::createReader(const dds::sub::qos::DataReaderQos & qos)
{
    
    if (_sample_reader != dds::core::null)
    {
        // We are calling into CycloneDDSSimulationBus::startBlockingReception to be released from the waitset
        // no synchronization needed because we are in the same thread
        if (_release_reader_conditions)
        {
            _release_reader_conditions();
        }

        // Now we need to close the old reader 
        _sample_reader->close();
        
    }

    // at this point we are overriding the existing reader, but it will not be deleted
    // because it's still part of the ReadCondition. But we are not affected from the old
    // reader because we have closed him.
    _sample_reader = dds::sub::DataReader<fep3::ddstypes::BusData>(_subscriber,
        _topic->getSampleTopic()
        , qos
         , this, dds::core::status::StatusMask::none()
        );

}

void ReaderItemQueue::setRecreateWaitSetCondition(const std::function<void()> & release_reader_conditions)
{
    _release_reader_conditions = release_reader_conditions;
}

size_t ReaderItemQueue::size() const
{
    try
    {
        auto sample_reader = _sample_reader;
        auto samples = sample_reader.select().state(dds::sub::status::DataState::new_data()).read();
        return samples.length(); //_sample_reader->qos()->datareader_cache_status().sample_count();
    }
    catch (dds::core::Exception & exception)
    {
        logError(convertExceptionToResult(exception));
    }
    return 0;
}

size_t ReaderItemQueue::capacity() const
{
    try
    {
        auto qos = _sample_reader->qos().delegate().ddsc_qos();
        return qos->history.depth;
    }
    catch (dds::core::Exception & exception)
    {
        logError(convertExceptionToResult(exception));
    }
    return 0;
}

void ReaderItemQueue::logError(const fep3::Result& res) const
{
    if (_logger)
    {
        if (_logger->isErrorEnabled())
        {
            _logger->logError(a_util::result::toString(res));
        }
    }
}

bool ReaderItemQueue::pop(fep3::ISimulationBus::IDataReceiver& receiver)
{
    try
    {
        return popFrom(_sample_reader, _streamtype_reader, _subscriber, receiver);
    }
    catch (dds::core::Exception & exception)
    {
        logError(convertExceptionToResult(exception));
    }
    return false;
}

bool ReaderItemQueue::popFrom
    (dds::sub::DataReader<fep3::ddstypes::BusData>& sample_reader
    , dds::sub::DataReader<fep3::ddstypes::StreamType>& streamtype_reader
    , const dds::sub::Subscriber& subscriber
    , fep3::ISimulationBus::IDataReceiver& receiver
    )
{
    if (!sample_reader.is_nil())
    {
        // TODO: No coherent access implemented in cxx api
        //subscriber.delegate()->begin_coherent_access();

        // readers contains a list of reader, i.e.:
        // _sample_reader
        // _sample_reader
        // _streamtype_reader
        // _sample_reader
        // _sample_reader
        // depending on the recieve order

        // TODO For now we are reading both streamtype and sample if a sample read is triggered

        auto samples = sample_reader.select().state(dds::sub::status::DataState::new_data()).take();
        bool read_something = false;
        for (auto sample : samples){
                read_something = true;
                receiver(createSample(sample, sample.info()));
            }
        for (auto streamtype : streamtype_reader.select()
            .max_samples(1)
            .take()){
            read_something = true;
            auto stream_type = createStreamType(streamtype, streamtype.info());
            receiver(stream_type);
            if (_topic->updateStreamType(*stream_type))
            {
                // first read all samples
                while (pop(receiver));

                // now recreate reader
                const auto qos = _subscriber.default_datareader_qos();
                createReader(qos);
            }
        }
            //subscriber.delegate()->end_coherent_access();
         return read_something;
    }
    //subscriber.delegate()->end_coherent_access();
    return false;
}

fep3::Optional<fep3::Timestamp> ReaderItemQueue::getFrontTime() const
{
    //dds::sub::CoherentAccess coherent_access(_subscriber);
    //_subscriber.delegate()->begin_coherent_access();

    std::vector<dds::sub::AnyDataReader> readers;
    dds::sub::DataReader<fep3::ddstypes::BusData> sa (_subscriber, _topic->getSampleTopic());
    sa->wrapper().select();
    auto streamtype_reader = _streamtype_reader;
    auto sample_reader = _sample_reader;

    auto sample = sample_reader->wrapper().select()
        .max_samples(1)
        .read().begin();

    return convertTimestamp(sample->info().timestamp());
     
    auto streamtype = streamtype_reader->wrapper().select()
        .max_samples(1)
        .read().begin();

    return convertTimestamp(streamtype->info().timestamp());
    
    //_subscriber.delegate()->end_coherent_access();

    return {};
}

dds::core::cond::Condition ReaderItemQueue::createSampleReadCondition
    (const std::shared_ptr<fep3::ISimulationBus::IDataReceiver>& receiver)
{
    return dds::sub::cond::ReadCondition
    (_sample_reader
        , dds::sub::status::SampleState::not_read()
        ,
            [sample_reader = _sample_reader
            , streamtype_reader = _streamtype_reader
            , subscriber = _subscriber
            , receiver
            , this](dds::core::cond::Condition condition) mutable
            {
                if (popFrom(sample_reader, streamtype_reader, subscriber, *receiver.get()))
                {
                    while (popFrom(sample_reader, streamtype_reader, subscriber, *receiver.get()));
                    return true;
                }
                else
                {
                    return false;
                }
            }
        );
}


dds::core::cond::Condition ReaderItemQueue::createStreamTypeReadCondition
    (const std::shared_ptr<fep3::ISimulationBus::IDataReceiver>& receiver)
{
    return dds::sub::cond::ReadCondition(_streamtype_reader, dds::sub::status::SampleState::not_read(),
        [sample_reader = _sample_reader
        , streamtype_reader = _streamtype_reader
        , subscriber = _subscriber
        , receiver
        , this
        ](dds::core::cond::Condition condition) mutable
        {
            if (this->popFrom(sample_reader, streamtype_reader, subscriber, *receiver.get()))
            {
                while (this->popFrom(sample_reader, streamtype_reader, subscriber, *receiver.get()));
                return true;
            }
            else
            {
                return false;
            }
        }
    );
}


void ReaderItemQueue::on_data_available(
    dds::sub::DataReader<fep3::ddstypes::BusData>& /*reader*/)
{
    std::cout << "           on_data_available" << std::endl;
}

void ReaderItemQueue::on_requested_deadline_missed(
    dds::sub::DataReader<fep3::ddstypes::BusData>& /*reader*/,
    const dds::core::status::RequestedDeadlineMissedStatus& /*status*/)
{
    std::cout << "           on_requested_deadline_missed" << std::endl;
}

void ReaderItemQueue::on_requested_incompatible_qos(
    dds::sub::DataReader<fep3::ddstypes::BusData>& /*reader*/,
    const dds::core::status::RequestedIncompatibleQosStatus& /*status*/)
{
    std::cout << "           on_requested_incompatible_qos" << std::endl;
}

void ReaderItemQueue::on_sample_rejected(
    dds::sub::DataReader<fep3::ddstypes::BusData>& /*reader*/,
    const dds::core::status::SampleRejectedStatus& /*status*/)
{
    std::cout << "           on_requested_incompatible_qos" << std::endl;
}

void ReaderItemQueue::on_liveliness_changed(
    dds::sub::DataReader<fep3::ddstypes::BusData>& /*reader*/,
    const dds::core::status::LivelinessChangedStatus& /*status*/)
{
    std::cout << "           on_requested_incompatible_qos" << std::endl;
}

void ReaderItemQueue::on_subscription_matched(
    dds::sub::DataReader<fep3::ddstypes::BusData>& /*reader*/,
    const dds::core::status::SubscriptionMatchedStatus& /*status*/)
{
    std::cout << "           on_subscription_matched" << std::endl;
}

void ReaderItemQueue::on_sample_lost(
    dds::sub::DataReader<fep3::ddstypes::BusData>& /*reader*/,
    const dds::core::status::SampleLostStatus& /*status*/)
{
    std::cout << "           on_sample_lost" << std::endl;
}