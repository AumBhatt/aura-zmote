// Boost C++ Libraries
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
// Standard C++ Libraries
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>

#define PORT 4998
#define TIMEOUT 10

namespace GC_IR {
    std::string learnIRCommand(std::string host_ip, int host_port);
    std::vector<std::string> formatIRCommand(std::string IR_response);
};

std::string GC_IR::learnIRCommand(std::string host_ip, int host_port = PORT) {
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
    // send "get_IRL" message
    boost::asio::write(
        socket,
        boost::asio::buffer("get_IRL\r"),
        error
    );

    // start learning thread
    boost::asio::streambuf recieve_buf;
    std::thread(
        [&]() {
            boost::asio::read(
                socket,
                recieve_buf,
                boost::asio::transfer_all(),
                error
            );
        }
    ).detach();

    // close socket on click
    std::cin.get();
    socket.close();
    
    return std::string(boost::asio::buffer_cast<const char*>(recieve_buf.data()));
}

std::vector<std::string> GC_IR::formatIRCommand(std::string IR_response) {
    std::vector<std::string> commands;
    std::string delim = "sendir";
    IR_response.erase(0, IR_response.find(delim));
    while(!IR_response.empty()) {
        size_t pos = IR_response.find(delim);
        commands.push_back(
            IR_response.substr(0, IR_response.find(delim, 1))
        );
        IR_response.erase(0, IR_response.find(delim, 1));
    }
    return commands;
}

int main() {
    std::string s =
    "IR Learner Enabledsendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692sendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692sendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692";

    std::vector<std::string> v = GC_IR::formatIRCommand(s);
    // s.erase(0, s.find("sendir"));
    // boost::algorithm::split(v, s, boost::is_any_of("sendir"));
    for(std::string i : v) {
        std::cout << std::endl << i << std::endl;
    }
    std::cout << v.size();
    return 0;
}