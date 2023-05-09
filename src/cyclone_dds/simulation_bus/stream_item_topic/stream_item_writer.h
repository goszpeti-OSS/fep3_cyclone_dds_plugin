

#pragma once

#include <fep3/components/simulation_bus/simulation_bus_intf.h>
#include "../eclipse_cyclone_dds_include.h"
#include "stream_item_topic.h"

class StreamItemDataWriter
    : public fep3::ISimulationBus::IDataWriter
    , public dds::pub::DataWriterListener<fep3::ddstypes::BusData>
{
public:
    StreamItemDataWriter(const std::shared_ptr<StreamItemTopic>& topic
        , size_t queue_capacity
    );
    ~StreamItemDataWriter();

    fep3::Result write(const fep3::IDataSample& data_sample) override;
    fep3::Result write(const fep3::IStreamType& stream_type) override;
    fep3::Result transmit() override;

    void createWriter(const dds::pub::qos::DataWriterQos& qos);
    bool wait_for_reader();

protected:
    void on_offered_deadline_missed(
        dds::pub::DataWriter<fep3::ddstypes::BusData>& writer
        , const dds::core::status::OfferedDeadlineMissedStatus& status) override;

    void on_offered_incompatible_qos(dds::pub::DataWriter<fep3::ddstypes::BusData>& writer
        , const dds::core::status::OfferedIncompatibleQosStatus& status) override;

    void on_liveliness_lost(
        dds::pub::DataWriter<fep3::ddstypes::BusData>& writer
        , const dds::core::status::LivelinessLostStatus& status) override;

    void on_publication_matched(
        dds::pub::DataWriter<fep3::ddstypes::BusData>& writer
        , const dds::core::status::PublicationMatchedStatus& status) override;

private:
    dds::pub::Publisher _publisher = dds::core::null;
    std::shared_ptr<StreamItemTopic> _topic;
    dds::pub::DataWriter<fep3::ddstypes::StreamType> _stream_type_writer = dds::core::null;
    dds::pub::DataWriter<fep3::ddstypes::BusData> _sample_writer = dds::core::null;

};

