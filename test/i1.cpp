// Boost C++ Libraries
#include <boost/asio.hpp>
// Standard C++ Libraries
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

#define PORT 4998
#define TIMEOUT 10

namespace GC_IR {
    std::vector<std::string> learnIRCommand(std::string host_ip, std::string host_port = PORT);
};

std::vector<std::string> GC_IR::learnIRCommand(std::string host_ip, std::string host_port = PORT) {
    // boost IO service
    boost::asio::io_service io_serv;
    // asio socket
    boost::asio::ip::tcp::socket socket(io_serv);
    // connect socket
    socket.connect(
        boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string(host_ip), host_port
        )
    );
    // system error handler
    boost::system::error_code error;
    // send request
}