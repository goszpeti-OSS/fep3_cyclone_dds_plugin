

#pragma once

#include <fep3/components/simulation_bus/simulation_bus_intf.h>
#include <fep3/components/simulation_bus/simulation_data_access.h>
#include <fep3/components/logging/logging_service_intf.h>
#include "../eclipse_cyclone_dds_include.h"
#include "stream_item_topic.h"
#include "../reader_item_queue.h"

class StreamItemDataReader
    : public fep3::arya::ISimulationBus::IDataReader
{
public:
    StreamItemDataReader(const std::shared_ptr<StreamItemTopic> & topic
                        , size_t queue_capacity
                        , const std::weak_ptr<fep3::base::SimulationDataAccessCollection<ReaderItemQueue>>& data_access_collection
                        , const std::shared_ptr<fep3::ILogger>& logger);
    ~StreamItemDataReader() override;

    size_t size() const override;
    size_t capacity() const override;
    bool pop(fep3::ISimulationBus::IDataReceiver& receiver) override;

    void reset(const std::shared_ptr<fep3::ISimulationBus::IDataReceiver>& receiver = {}) override;

    fep3::Optional<fep3::Timestamp> getFrontTime() const override;

protected:
    const std::shared_ptr<fep3::ILogger> _logger;
private:
    std::shared_ptr<StreamItemTopic> _topic;
    const std::shared_ptr<ReaderItemQueue> _item_queue;
    // the reader does not take (permanent) ownership of the data access collection -> weak_ptr
    std::weak_ptr<fep3::base::SimulationDataAccessCollection<ReaderItemQueue>> _data_access_collection;
    fep3::Optional<fep3::base::SimulationDataAccessCollection<ReaderItemQueue>::const_iterator> _data_access_iterator;

};
