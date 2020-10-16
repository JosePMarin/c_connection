#include "common.hpp"


class Connect
{
    private:

        std::mutex mux_queue;
        std::string m_url;
        asio::error_code m_ec;
        asio::ip::tcp::resolver::iterator m_endpoint_iterator;
        std::thread m_thread_context;
        asio::io_context m_context;
        std::shared_ptr<asio::ip::tcp::socket> p_socket = nullptr;
        
        
        void _grab_some_data(std::vector<char> &read_buffer, std::shared_ptr<asio::ip::tcp::socket> socket)
        {
            std::scoped_lock lock(mux_queue);
            try
            {
                socket->async_read_some(asio::buffer(read_buffer.data(), read_buffer.size()),
                [&](std::error_code m_ec, std::size_t length)
                {
                    if (!m_ec)
                    {
                        std::cout << "\n\nRead " << length << " bytes\n\n";
                        for (int i = 0; i < length; i++)
                            std::cout << read_buffer[i];
                        
                        _grab_some_data(read_buffer, socket);
                    }
                });
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            std::cout << "Data read" << std::endl;           
        }

    public:
        
        Connect(std::string url) : m_url(url)
        {
            // Launch the context (platform specific interface) into a thread and keep it alive so that it does not finish
            asio::io_context::work idle_work(m_context);
            m_thread_context = std::thread([&](){ m_context.run(); });
            // Get a list of endpoints corresponding to the server name
            asio::ip::tcp::resolver resolver(m_context);
            asio::ip::tcp::resolver::query query (m_url, "http");
            m_endpoint_iterator = resolver.resolve(query);
            p_socket = std::make_shared<asio::ip::tcp::socket>(m_context);
            try
            {
                asio::connect(*p_socket, m_endpoint_iterator, m_ec);
            }
            catch(const std::exception& e)
            {
                std::cout << "Failed to connect to adress:\n" << m_ec.message() << std::endl;
                std::cerr << e.what() << '\n';
            }
            if (!m_ec)
            {
                std::cout << "Connected!" << std::endl;
            }
            
        }

        virtual ~Connect()
        {
        }

        std::shared_ptr<asio::ip::tcp::socket> get_socket()
        {   
            return p_socket;
        }

        void get_data(std::string path, std::vector<char>& read_buffer)
        {
            std::shared_ptr<asio::ip::tcp::socket> _socket = get_socket();
            
            // Form the request, stablishing Connection:close to close the socket after transmiting
            if (_socket->is_open())
            {
                _grab_some_data(read_buffer, _socket);

                asio::streambuf bRequest;
                std::ostream request_stream(&bRequest);
                request_stream << "GET /" << path << " HTTP/1.0\r\n";
                request_stream << "Host: " << m_url << "\r\n";
                request_stream << "Accept: */*\r\n";
                request_stream << "Connection: close\r\n\r\n";;

                // Send the request
                int i = _socket->write_some(asio::buffer(bRequest.data(), bRequest.size()), m_ec);

                // Program does something while in the other thread data is captured
                std::this_thread::sleep_for(std::chrono::seconds(5));

                m_context.stop();
                if (m_thread_context.joinable()) m_thread_context.join();
            }
        }
};
// std::vector<char> read_buffer(20 * 1024);
// void _grab_some_data(asio::ip::tcp::socket& socket)
// {
//     socket.async_read_some(asio::buffer(read_buffer.data(), read_buffer.size()),
//         [&](std::error_code ec, std::size_t length)
//         {
//             if (!ec)
//             {
//                 std::cout << "\n\nRead " << length << " bytes\n\n";
//                 for (int i = 0; i < length; i++)
//                     std::cout << read_buffer[i];
                
//                 _grab_some_data(socket);
//             }
//         }
//     );
// }
int main()
{
    std::string url = "51.38.81.49";
    std::string path = "index.html";
    std::vector<char> read_buffer(20 * 1024);
    Connect con = Connect(url);
    con.get_data(path, read_buffer);
    std::cout<<"debug"<<std::endl;


    // std::string url = "51.38.81.49";
    // std::string path = "index.html";
    // asio::error_code ec;
    // asio::io_context context;
    

    // // Launch the context (platform specific interface) into a thread and keep it alive so that it does not finish
    // asio::io_context::work idle_worl(context);
    // std::thread thread_context = std::thread([&](){ context.run(); });

    // // Get a list of endpoints corresponding to the server name
    // asio::ip::tcp::resolver resolver(context);
    // asio::ip::tcp::resolver::query query (url, "http");
    // asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    // //asio::ip::tcp::endpoint endpoint(asio::ip::make_address("151.101.121.50", ec), 80);
    // //Try each endpoint until we successfully establish a connection
    // asio::ip::tcp::socket socket(context);
    // asio::connect(socket, endpoint_iterator, ec);

    // if (!ec)
    // {
    //     std::cout << "Connected!" << std::endl;
    // }
    // else
    // {
    //     std::cout << "Failed to connect to adress:\n" << ec.message() << std::endl;
    // }

    // // Form the request, stablishing Connection:close to close the socket after transmiting
    // if (socket.is_open())
    // {
    //     _grab_some_data(socket);

    //     asio::streambuf bRequest;
    //     std::ostream request_stream(&bRequest);
    //     request_stream << "GET /" << path << " HTTP/1.0\r\n";
    //     request_stream << "Host: " << url << "\r\n";
    //     request_stream << "Accept: */*\r\n";
    //     request_stream << "Connection: close\r\n\r\n";;

    //     // Send the request
    //     int i = socket.write_some(asio::buffer(bRequest.data(), bRequest.size()), ec);

    //     // Program does something while in the other thread data is captured
    //     std::this_thread::sleep_for(std::chrono::seconds(20));

    //     context.stop();
    //     if (thread_context.joinable()) thread_context.join();

        

    //     // Read the response and print it
        
        
    //     // size_t bytes = socket.available();
    //     // std::cout << "Bytes Available: " << bytes << std::endl;
    //     // if (bytes > 0)
    //     // {
    //     //     //std::vector<char> response(bytes);
    //     //     std::vector<char> response(bytes);
    //     //     size_t byte_received = socket.read_some(asio::buffer(response.data(), response.size()), ec); 
    //     //     // Check that response is OK and status code
    //     //     std::string http_version(response.begin(), response.begin() + 8);
    //     //     std::string status_code(response.begin() + 9, response.begin() + 12);
    //     //     // size_t i = http_version.find("HTTP/");
    //     //     // size_t j = status_code.find("200");
    //     //     if (http_version.find("HTTP/") != std::string::npos && status_code.find("200") != std::string::npos) 
    //     //     {
    //     //         std::cout << "status code " << status_code;
                
    //     //     }
    //     //     else 
    //     //     {
    //     //         std::cout << "Invalid Response - status code = " << status_code;
    //     //     }
    //     //     for (auto c : response)
    //     //             std::cout << c;
    //     // }

        
    // }
    // std::cout << "debug" << std::endl;
    // return 0; 

}