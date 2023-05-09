

#pragma once

#include <fep3/components/simulation_bus/simulation_bus_intf.h>
#include <fep3/components/simulation_bus/simulation_data_access.h>
#include "reader_item_queue.h"

/**
 * Topic provides standardizes access to all topic types
 */
class ITopic
{
protected:
    /// DTOR
    ~ITopic() = default;

public:
    virtual std::string GetTopic() = 0;
    virtual std::unique_ptr<fep3::arya::ISimulationBus::IDataReader> createDataReader
        (size_t queue_capacity
        , const std::weak_ptr<fep3::base::SimulationDataAccessCollection<ReaderItemQueue>>& data_access_collection
        ) = 0;
    virtual std::unique_ptr<fep3::arya::ISimulationBus::IDataWriter> createDataWriter(size_t queue_capacity) = 0;
};
