

#pragma once

#include "../topic_intf.h"

#include <queue>
#include <mutex>

/*
* Internal simulation bus topic can be used to make information available via ISimulationBus interface
* Data will be transported via a fifo queue. InternalTopic only supports IDataReader.
*/
class InternalTopic :
    public std::enable_shared_from_this<InternalTopic>,
    public ITopic
{
private:

    class InternalReader
        : public fep3::arya::ISimulationBus::IDataReader
    {
    public:
        InternalReader(const std::shared_ptr<InternalTopic> & internal_topic);
        ~InternalReader() = default;

        size_t size() const override;
        size_t capacity() const override;
        bool pop(fep3::ISimulationBus::IDataReceiver& receiver) override;

        void reset(const std::shared_ptr<fep3::arya::ISimulationBus::IDataReceiver>& receiver) override;

        fep3::Optional<fep3::Timestamp> getFrontTime() const override;

    private:
        std::shared_ptr<InternalTopic> _internal_topic;
    };

public:
    InternalTopic(const std::string & topic_name);
    ~InternalTopic() = default;

    void write(const std::string& data);

    std::string GetTopic() override;

    std::unique_ptr<fep3::ISimulationBus::IDataReader> createDataReader
        (size_t queue_capacity
        , const std::weak_ptr<fep3::base::SimulationDataAccessCollection<ReaderItemQueue>>& data_access_collection
        ) override;
    std::unique_ptr<fep3::ISimulationBus::IDataWriter> createDataWriter(size_t queue_capacity) override;

private:
    std::string _topic_name;
    std::queue<std::string> _queue;
    std::recursive_mutex _queue_mutex;
    friend InternalReader;
};
