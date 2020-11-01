#include "utils.hpp"
#include "connection.hpp"

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