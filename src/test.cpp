#define ASIO_STANDALONE

#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>

class Connection 
{
private:

    std::string m_url;
    std::string m_path;
    std::shared_ptr<asio::ip::tcp::socket> m_socket_connection;
    std::mutex m_mutex;
    std::vector<char> m_read_buffer;

public:
    
    
    asio::error_code ec;
    std::thread thread_context;
    std::shared_ptr<asio::io_context> context;
    std::shared_ptr<asio::io_context::work> idle_work;

    Connection(std::string url, std::string path) : m_url{url}, m_path{path}
    {
        context = std::make_shared<asio::io_context>();
        // Launch the context (platform specific interface) into a thread and keep it alive so that it does not finish
        idle_work = std::make_shared<asio::io_context::work> (*context);
        thread_context = std::thread([&](){ context->run(); });
        // Get a list of endpoints corresponding to the server name
        asio::ip::tcp::resolver resolver(*context);
        asio::ip::tcp::resolver::query query (m_url, "http");
        asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        //Try each endpoint until we successfully establish a connection
        m_socket_connection = std::make_shared<asio::ip::tcp::socket>(*context);
        asio::connect(*m_socket_connection, endpoint_iterator, ec);
        m_read_buffer.resize(20 * 1024);
        m_mutex.lock();
        if (!ec)
        {
            
            std::cout << "Connected!" << std::endl;
        }
        else
        {
            std::cout << "Failed to connect to adress:\n" << ec.message() << std::endl;
        }
        m_mutex.unlock();
    }

    ~Connection(){}

    void stop_connection()
    {
        m_socket_connection->close();
        context->stop();
        if (thread_context.joinable()) thread_context.join();
    }

    std::shared_ptr<asio::ip::tcp::socket> get_socket()
    {
        return m_socket_connection;
    }

    void grab_some_data()
    {
        m_socket_connection->async_read_some(asio::buffer(m_read_buffer.data(), m_read_buffer.size()),
            [&](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    std::cout << "\n\nRead " << length << " bytes\n\n";
                    for (int i = 0; i < length; i++)
                        std::cout << m_read_buffer[i];
                    grab_some_data();
                }
            }
        );
    }
};

// void WorkerThread(std::shared_ptr<asio::io_context> context)
// {
//     global_mutex.lock();
//     std::cout << "[" << std::this_thread::get_id() <<"] Thread Start" << std::endl;
//     global_mutex.unlock();
//     context->run();
//     global_mutex.lock();
//     std::cout << "[" << std::this_thread::get_id() <<"] Thread Finish" << std::endl;
//     global_mutex.unlock();
// }

int main()
{
    std::string url = "51.38.81.49";
    std::string path = "index.html";
    Connection con(url, path);
    auto connection_socket = con.get_socket();

    // Form the request, stablishing Connection:close to close the socket after transmiting
    if (connection_socket->is_open())
    {
        con.grab_some_data();

        asio::streambuf bRequest;
        std::ostream request_stream(&bRequest);
        request_stream << "GET /" << path << " HTTP/1.0\r\n";
        request_stream << "Host: " << url << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";;
        // Send the request
        try
        {
            int i = connection_socket->write_some(asio::buffer(bRequest.data(), bRequest.size()), con.ec);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        // Program does something while in the other thread data is captured
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Read the response and print it
        size_t bytes = connection_socket->available();
        std::cout << "Bytes Available: " << bytes << std::endl;   
    }
    con.stop_connection();
    return 0; 
}