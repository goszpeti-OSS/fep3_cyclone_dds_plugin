


#include "../vector_raw_memory.hpp"
#include "../converter.h"

#include "stream_item_writer.h"

using namespace dds::core;
using namespace dds::pub;
using namespace dds::pub::qos;

using namespace fep3;

StreamItemDataWriter::StreamItemDataWriter(const std::shared_ptr<StreamItemTopic> & topic
    , size_t) /*queue_capacity*/
    : _topic(topic)
    , _publisher(topic->getDomainParticipant())
    , _stream_type_writer(DataWriter<fep3::ddstypes::StreamType>(_publisher
        , topic->getStreamTypeTopic(), _publisher.default_datawriter_qos()))
{
    _stream_type_writer->set_batch(true); // for flush to have an effect
    createWriter(_publisher.default_datawriter_qos());
}

StreamItemDataWriter::~StreamItemDataWriter()
{
    _stream_type_writer.close();
    _sample_writer.close();
}

struct BytesTopicTypeRawMemory : public fep3::arya::IRawMemory
{

public:
    BytesTopicTypeRawMemory(fep3::ddstypes::BusData & value) : _value(value)
    {
    }
    size_t capacity() const override
    {
        return _value.data().capacity();
    }
    const void* cdata() const override
    {
        return nullptr;
    }
    size_t size() const override
    {
        return _value.data().size();
    }
    size_t set(const void* data, size_t data_size) override
    {
        std::vector<uint8_t> buffer(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + data_size);
        _value.data(buffer);
        return data_size;
    }
    size_t resize(size_t data_size) override
    {
        return data_size;
    }

private:
    fep3::ddstypes::BusData& _value;

};

void StreamItemDataWriter::createWriter(const dds::pub::qos::DataWriterQos & qos)
{
    if(_sample_writer != dds::core::null)
    {
        _sample_writer->close();
    }
    _sample_writer = DataWriter<fep3::ddstypes::BusData>(_publisher
        , _topic->getSampleTopic()
        , qos
        , this
        , dds::core::status::StatusMask::none());
    _sample_writer.enable();
    _sample_writer->set_batch(true);
}

fep3::Result StreamItemDataWriter::write(const IDataSample& data_sample)
{
    try
    {
        fep3::ddstypes::BusData sample;
        BytesTopicTypeRawMemory raw_memory(sample);
        data_sample.read(raw_memory);

        _sample_writer.write(sample, convertTimestamp(data_sample.getTime()));
        
        return {};
    }
    catch (Exception & exception)
    {
        return convertExceptionToResult(exception);
    }

}

fep3::Result StreamItemDataWriter::write(const IStreamType& stream_type)
{
    try
    {
        fep3::ddstypes::StreamType dds_stream_type;
        dds_stream_type.metatype(stream_type.getMetaTypeName());

        for (auto property_name : stream_type.getPropertyNames())
        {
            dds_stream_type.properties().push_back(fep3::ddstypes::Property
            (property_name
                , stream_type.getPropertyType(property_name)
                , stream_type.getProperty(property_name)));
        }

        _stream_type_writer.write(dds_stream_type);

        if (_topic->updateStreamType(stream_type))
        {
            const auto qos = _publisher.default_datawriter_qos();
            createWriter(qos);
        }

        return {};
    }
    catch (Exception& exception)
    {
        return convertExceptionToResult(exception);
    }
}


 bool StreamItemDataWriter::wait_for_reader()
 {
    dds::core::cond::StatusCondition sc = dds::core::cond::StatusCondition(_sample_writer);
    sc.enabled_statuses(dds::core::status::StatusMask::publication_matched());

    dds::core::cond::WaitSet waitset;
    waitset.attach_condition(sc);

    dds::core::cond::WaitSet::ConditionSeq conditions =
        waitset.wait(dds::core::Duration::from_secs(2));

    for (const auto& c : conditions)
    {
        if (c == sc)
            return true;
    }

    std::cout << "Did not discover a reader.\n" << std::flush;

    return false;
}


fep3::Result StreamItemDataWriter::transmit()
{
    try
    {
        _stream_type_writer.delegate()->write_flush();
        _sample_writer.delegate()->write_flush();

        
        //The behavior of transmit() is not finally defined. Maybe we need to block until data was send
       /* _stream_type_writer->wait_for_acknowledgments(dds::core::Duration(0, 1000000));
        _sample_writer->wait_for_acknowledgments(dds::core::Duration(0, 1000000));
        */
        return {};
    }
    catch (Exception & exception)
    {
        return convertExceptionToResult(exception);
    }

}

void StreamItemDataWriter::on_offered_deadline_missed(
    dds::pub::DataWriter<fep3::ddstypes::BusData>& /*writer*/,
    const dds::core::status::OfferedDeadlineMissedStatus& /*status*/)
{
    //std::cout << "           on_offered_deadline_missed" << std::endl;
}
void StreamItemDataWriter::on_offered_incompatible_qos(
    dds::pub::DataWriter<fep3::ddstypes::BusData>& /*writer*/,
    const dds::core::status::OfferedIncompatibleQosStatus& /*status*/)
{
    //std::cout << "           on_offered_incompatible_qos" << std::endl;
}
void StreamItemDataWriter::on_liveliness_lost(
    dds::pub::DataWriter<fep3::ddstypes::BusData>& /*writer*/,
    const dds::core::status::LivelinessLostStatus& /*status*/)
{
    //std::cout << "           on_liveliness_lost" << std::endl;
}
void StreamItemDataWriter::on_publication_matched(
    dds::pub::DataWriter<fep3::ddstypes::BusData>& /*writer*/,
    const dds::core::status::PublicationMatchedStatus& /*status*/)
{
    //std::cout << "           on_publication_matched" << std::endl;
}