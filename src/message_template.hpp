#pragma once
#include "message_template.hpp"

namespace com
{
    // Header is sent at the start of the message 
    template <typename T>
    struct header
    {
        T id{};
        uint32_t size = 0;
    };

    template <typename T>
    struct content
    {
        header<T> message_header {};
        std::vector<uint8_t> body;
         
        size_t size() const
        {
            return sizeof(header<T>) + body.size();
        }

        // Override for std::cout compatibility - produces friendly description of message
        friend std::ostream& operator << (std::ostream& os, const content<T>& msg)
        {
            os << "ID:" << int(msg.message_header.id) << " Size:" << msg.message_header.size;
            return os;
        }

        // Pushes any POD-like data into the message buffer
        template<typename DataType>
        friend content<T>& operator << (content<T>& msg, DataType& data)
        {
            // Check that the type of data pushed is copyable
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");

            // Cache location towards the end of the vector where the pulled data starts
            size_t size = msg.body.size() - sizeof(DataType);

            // Physically copy the data from the vector into the user variable
            std::memcpy(&data, msg.body.data() + size, sizeof(DataType));

            // Readapt vector size, deleting read bytes and reseting end position
            msg.body.resize(size);

            // Recalculate message size
            msg.message_header.size = msg.size();

            // Return the target message so it can be "chained"
            return msg;
        }
    };

    template <typename T>
    class connection;

    template <typename T> 
    struct owned_message
    {
        std::shared_ptr<connection<T>> remote = nullptr;
        content<T> msg;
    };
}