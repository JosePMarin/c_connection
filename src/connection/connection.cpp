#include "connection.hpp"

Connection::Connection(std::string url, std::string path) : m_url{url}, m_path{path}
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

Connection::~Connection(){}

void Connection::stop_connection()
{
    m_socket_connection->close();
    context->stop();
    if (thread_context.joinable()) thread_context.join();
}

std::shared_ptr<asio::ip::tcp::socket> Connection::get_socket()
{
    return m_socket_connection;
}

void Connection::grab_some_data()
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
