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

    try {
        // connect socket
        socket.connect(
            boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string(host_ip), host_port
            )
        );
        std::cout << "\nConnected\n";
        // system error handler
        boost::system::error_code error;
        // send "get_IRL" message
        boost::asio::write(
            socket,
            boost::asio::buffer("get_IRL\r"),
            error
        );
        if(error) {
            throw "Unable to start learning mode";
        }

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
         if(error) {
             throw "Unable to receive socket stream messages.";
         }

        // close socket on click
        // sleep(10);
        std::cin.get();
        boost::asio::write(
            socket,
            boost::asio::buffer("stop_IRL\r"),
            error
        );
/*         boost::asio::streambuf close_buf;
        boost::asio::read(
            socket,
            close_buf,
            boost::asio::transfer_all(),
            error
        );
        if(error) {
            throw "Unable to stop learning mode.";
        }
        std::cout << std::endl << std::string(boost::asio::buffer_cast<const char*>(close_buf.data())) << std::endl;
 */
        socket.close();
        return std::string(boost::asio::buffer_cast<const char*>(recieve_buf.data()));
    }
    catch(const char *ex) {
        std::cout << std::endl << "\n GC_IR: " << ex << std::endl;
        socket.~basic_stream_socket();
        return "";
    }
}

std::vector<std::string> GC_IR::formatIRCommand(std::string IR_response) {
    std::vector<std::string> commands;
    std::string delim = "sendir";
    
    try {
        // Remove 'IR Learner Enabled'
        // IR_response.erase(0, std::string("IR Learner Enabled").length());
        if(IR_response.find("IR Learner Enabled") != std::string::npos)
            boost::erase_all(IR_response, "IR Learner Enabled");

        // Remove 'IR Learner Disabled'
        if(IR_response.find("IR Learner Disabled") != std::string::npos)
            boost::erase_all(IR_response, "IR Learner Disabled");
            // IR_response.erase(
            //     IR_response.find("IR Learner Disabled"),
            //     IR_response.length()
            // );

        // Segregate the commands into a vector
        while(!IR_response.empty()) {
            size_t pos = IR_response.find(delim);
            commands.push_back(
                IR_response.substr(0, IR_response.find(delim, 1))
            );
            IR_response.erase(0, IR_response.find(delim, 1));
        }
    }
    catch(std::exception &e) {
        std::cout << std::endl << e.what() << std::endl;
    }
    return commands;
}

int main() {
    std::string s =
    "IR Learner Enabledsendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692\rsendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692sendir\r,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692\rIR Learner Disabled";

    // std::vector<std::string> v = GC_IR::formatIRCommand(s);
    std::string ss = GC_IR::learnIRCommand("127.0.0.1", 3000);
    // std::cout <<  std::endl << ss << std::endl;
    std::vector<std::string> v = GC_IR::formatIRCommand(ss);
    try{
        for(std::string i : v) {
            std::cout << std::endl << i << std::endl;
        }
    }
    catch(std::exception &e) {
        std::cout << std::endl << e.what() << std::endl;
    }

    // std::cout << GC_IR::learnIRCommand("127.0.0.1", 3000);
    // v.~vector();

    return 0;
}