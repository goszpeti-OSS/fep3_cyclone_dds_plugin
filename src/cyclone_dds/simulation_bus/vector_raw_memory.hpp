

#pragma once

#include <fep3/base/sample/raw_memory_intf.h>
#include <vector>
#include <cstring>

/**
 * Converts a std::vector<uint8_t> into a IRawMemory interface
 */
struct VectorRawMemory: public fep3::arya::IRawMemory
{

public:
    VectorRawMemory(std::vector<uint8_t> & value) : _value(value)
    {
    }
    size_t capacity() const override
    {
        return _value.capacity();
    }
    const void* cdata() const override
    {
        return _value.data();
    }
    size_t size() const override
    {
        return _value.size();
    }
    size_t set(const void* data, size_t data_size) override
    {
        if (capacity() != data_size)
        {
            _value.resize(data_size);
        }

        //@TODO Absichern
        std::memcpy(_value.data(), data, data_size);
        return size();
    }
    size_t resize(size_t data_size) override
    {
        _value.resize(data_size);
        return capacity();
    }

private:
    std::vector<uint8_t>& _value;

};
