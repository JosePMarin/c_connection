#include "common.hpp"
#include "connection.hpp"

int main()
{
    std::string url = "example.com";
    std::string path = "index.html";

    Connection con(url, path);

    auto connection_socket = con.get_socket();
    con.print_to_console(con.get_request());

    con.stop_connection();

    return 0; 
}