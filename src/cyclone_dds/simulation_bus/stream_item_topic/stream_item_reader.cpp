


#include "stream_item_reader.h"

using namespace fep3;

StreamItemDataReader::StreamItemDataReader(const std::shared_ptr<StreamItemTopic> & topic
    , size_t /*queue_capacity*/
    , const std::weak_ptr<base::SimulationDataAccessCollection<ReaderItemQueue>>& data_access_collection
    , const std::shared_ptr<fep3::ILogger>& logger)
    : _topic(topic)
    , _item_queue(std::make_shared<ReaderItemQueue>
        (_logger
        , topic
        ))
    , _data_access_collection{data_access_collection}
    , _logger(logger)

{
    if (!_logger)
    {
        throw std::invalid_argument("logger argument is null");
    }
}

StreamItemDataReader::~StreamItemDataReader()
{}

size_t StreamItemDataReader::size() const
{
    return _item_queue->size();
}

size_t StreamItemDataReader::capacity() const
{
    return _item_queue->capacity();
}

bool StreamItemDataReader::pop(fep3::ISimulationBus::IDataReceiver& receiver)
{
    return _item_queue->pop(receiver);
}

void StreamItemDataReader::reset(const std::shared_ptr<fep3::ISimulationBus::IDataReceiver>& receiver)
{
    const auto& data_access_collection = _data_access_collection.lock();
    // remove the previous receiver (if any)
    if(_data_access_iterator)
    {
        if(data_access_collection)
        {
            data_access_collection->remove(_data_access_iterator.value());
        }
        _data_access_iterator.reset();
        if (_logger->isDebugEnabled())
        {
            _logger->logDebug(a_util::strings::format("Replaced already registered data receiver for reader from topic '%s'.", _topic->GetTopic().c_str()));
        }
    }

    if(data_access_collection && receiver)
    {
        // add the new receiver
        _data_access_iterator = data_access_collection->add(receiver, _item_queue);
    }
}

Optional<Timestamp> StreamItemDataReader::getFrontTime() const
{
    return _item_queue->getFrontTime();
}
