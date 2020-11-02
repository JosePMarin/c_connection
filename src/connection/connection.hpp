#include "common.hpp"

class Connection 
{
private:

    std::string m_url;
    std::string m_path;
    std::shared_ptr<asio::ip::tcp::socket> m_socket_connection;
    std::mutex m_mutex;
    std::vector<char> m_read_buffer;
    std::map<std::vector<char>, int>  grab_some_data(); 

public:
    
    
    asio::error_code ec;
    std::thread thread_context;
    std::shared_ptr<asio::io_context> context;
    std::shared_ptr<asio::io_context::work> idle_work;

    Connection(std::string url, std::string path); 

    ~Connection();

    void stop_connection(); 

    std::shared_ptr<asio::ip::tcp::socket> get_socket(); 

    std::map<std::vector<char>, int> get_request();

    void print_to_console(std::map<std::vector<char>, int> request);
};