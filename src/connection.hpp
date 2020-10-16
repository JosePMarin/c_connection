#pragma once

#include "common.hpp"
#include "message_template.hpp"
#include "queue.hpp"

namespace com
{
    template <typename T>
    class Connection : public std::enable_shared_from_this<connection<T>>
    {
        public:
            Connection()
            {
                ;
            }

            virtual ~Connection(){}

            bool connect_to_server();
            bool disconnect();
            bool is_connected();
            bool send(const content<T>& msg);
        
        protected:
        // Each connection has a unique socket to a remote
        asio::ip::tcp::socket socket;

        // This context is shared with the whole asio instance
        asio::io_context& asio_context;

        // Queue to hold all messages to be sent to the remote side
        Queue<content<T>> queue_message_out;

        // Queue to hold all message that have been received from remote
        Queue<owned_message>& queue_message_in;
    };
}