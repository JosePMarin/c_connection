#pragma once

#include "common.hpp"
#include "message_template.hpp"
#include "queue.hpp"
#include "connection.hpp"

namespace com
{
    template<typename T>
    class Client
    {
        public:

            Client() : socket(context)
            {
                // Initialise the socket with the io context
            }

            virtual ~Client()
            {
                disconnect();
            }
            
            bool connect(const std::string& host, const uint16_t port)
            {
                try
                {
                    connection = std::make_unique<connection<T>>();

                    asio::ip::tcp::resolver resolver(context);
                    endpoints = resolver.resolve(host, std::to_string(port));
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Client exception: " << e.what() << '\n';
                }
                
                return false;
            }

            bool disconnect()
            {

            }

            bool is_connected()
            {
                if (connection)
                {
                    return connection->is_connected();
                } else {
                    return false;
                }
                    

            }

            // Retrieve queue of messages from server
            Queue<owned_message<T>>& incoming()
            {
                return queue_message_in;
            }

        protected:
            asio::io_context context;
            std::thread thread_context;
            asio::ip::tcp::socket socket;
            std::unique_ptr<connection<T>> connection;

        private:
            Queue<owned_message<T>> queue_message_in;
    }
}