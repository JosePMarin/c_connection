#define ASIO_STANDALONE

#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

std::vector<char> read_buffer(20 * 1024);
void grab_some_data(asio::ip::tcp::socket& socket)
{
    socket.async_read_some(asio::buffer(read_buffer.data(), read_buffer.size()),
        [&](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::cout << "\n\nRead " << length << " bytes\n\n";
                for (int i = 0; i < length; i++)
                    std::cout << read_buffer[i];
                
                grab_some_data(socket);
            }
        }
    );
}


int main()
{
    std::string url = "51.38.81.49";
    std::string path = "index.html";
    asio::error_code ec;
    asio::io_context context;

    // Launch the context (platform specific interface) into a thread and keep it alive so that it does not finish
    asio::io_context::work idle_worl(context);
    std::thread thread_context = std::thread([&](){ context.run(); });

    // Get a list of endpoints corresponding to the server name
    asio::ip::tcp::resolver resolver(context);
    asio::ip::tcp::resolver::query query (url, "http");
    asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    //asio::ip::tcp::endpoint endpoint(asio::ip::make_address("151.101.121.50", ec), 80);
    //Try each endpoint until we successfully establish a connection
    asio::ip::tcp::socket socket(context);
    asio::connect(socket, endpoint_iterator, ec);

    if (!ec)
    {
        std::cout << "Connected!" << std::endl;
    }
    else
    {
        std::cout << "Failed to connect to adress:\n" << ec.message() << std::endl;
    }

    // Form the request, stablishing Connection:close to close the socket after transmiting
    if (socket.is_open())
    {
        grab_some_data(socket);

        asio::streambuf bRequest;
        std::ostream request_stream(&bRequest);
        request_stream << "GET /" << path << " HTTP/1.0\r\n";
        request_stream << "Host: " << url << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";;

        // Send the request
        int i = socket.write_some(asio::buffer(bRequest.data(), bRequest.size()), ec);

        // Program does something while in the other thread data is captured
        std::this_thread::sleep_for(std::chrono::seconds(20));

        context.stop();
        if (thread_context.joinable()) thread_context.join();

        

        // Read the response and print it
        
        
        // size_t bytes = socket.available();
        // std::cout << "Bytes Available: " << bytes << std::endl;
        // if (bytes > 0)
        // {
        //     //std::vector<char> response(bytes);
        //     std::vector<char> response(bytes);
        //     size_t byte_received = socket.read_some(asio::buffer(response.data(), response.size()), ec); 
        //     // Check that response is OK and status code
        //     std::string http_version(response.begin(), response.begin() + 8);
        //     std::string status_code(response.begin() + 9, response.begin() + 12);
        //     // size_t i = http_version.find("HTTP/");
        //     // size_t j = status_code.find("200");
        //     if (http_version.find("HTTP/") != std::string::npos && status_code.find("200") != std::string::npos) 
        //     {
        //         std::cout << "status code " << status_code;
                
        //     }
        //     else 
        //     {
        //         std::cout << "Invalid Response - status code = " << status_code;
        //     }
        //     for (auto c : response)
        //             std::cout << c;
        // }

        
    }
    std::cout << "debug" << std::endl;
    return 0; 

}