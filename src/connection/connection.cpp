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
    if (listener_thread.joinable()) listener_thread.join();
}

std::shared_ptr<asio::ip::tcp::socket> Connection::get_socket()
{
    return m_socket_connection;
}

void Connection::grab_some_data(std::map<std::vector<char>, int> &container_message)
{
    m_socket_connection->async_read_some(asio::buffer(m_read_buffer.data(), m_read_buffer.size()),
        [&](std::error_code ec, std::size_t length)
        {
            std::scoped_lock lock(m_mutex);
            if (!ec)
            {
                std::cout << "\n\nRead " << length << " bytes\n\n";
                container_message.insert({m_read_buffer, length});
                grab_some_data(container_message);
            } 
            m_cv.notify_all();
        }
    );
}

std::map<std::vector<char>, int> Connection::get_request()
{
    std::map<std::vector<char>, int> container_message;
    listener_thread = std::thread(&Connection::grab_some_data, this, std::ref(container_message));
    asio::streambuf bRequest;
    std::ostream request_stream(&bRequest);
    request_stream << "GET /" << m_path << " HTTP/1.0\r\n";
    request_stream << "Host: " << m_url << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";;
    // Send the request
    try
    {
        int i = m_socket_connection->write_some(asio::buffer(bRequest.data(), bRequest.size()), ec);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    // Wait for data
    std::unique_lock<std::mutex> lck(shared_mut);
    m_cv.wait(lck);

    // Read the response and print it
    size_t bytes = m_socket_connection->available();
    std::cout << "Bytes Available: " << bytes << std::endl;

    return container_message;
}

void Connection::print_to_console(std::map<std::vector<char>, int> request)
{
    std::scoped_lock lock(m_mutex);
    // Form the request, stablishing Connection:close to close the socket after transmiting
    if (m_socket_connection->is_open())
    {
        for (auto const& [read_buffer, length] : request)
        {
            for (int i = 0; i < length; i++)
                std::cout << read_buffer[i];
        }   
    }
}