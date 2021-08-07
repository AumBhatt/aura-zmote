// Boost C++ Libraries
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
// Standard C++ Libraries
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <stdexcept>
#include <iomanip>

#define PORT 4998
#define TIMEOUT 10

namespace GC_IR {
    std::string _learnIRCommand(std::string host_ip, int host_port = 4998);
    std::string cleanseIRCommand(std::string IR_response);
};

namespace GwUtils {
    std::string integerToHexString(int val, int width=4);
}

std::string GwUtils::integerToHexString(int val, int width)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(width) << std::uppercase << std::hex << val;
    return stream.str();
}
/*
 * std::string GC_IR::learnIRCommand(std::string host_ip, int host_port = PORT) {
 * 
 *     // boost IO service
 *     // io_service is deprecated now, should use io_context
 *     boost::asio::io_service io_serv;
 *     // asio socket
 *     boost::asio::ip::tcp::socket socket(io_serv);
 * 
 *     try {
 *         // connect socket
 *         socket.connect(
 *             boost::asio::ip::tcp::endpoint(
 *                 boost::asio::ip::address::from_string(host_ip), host_port
 *             )
 *         );
 *         // system error handler
 *         boost::system::error_code getIRL_error;
 *         // send "get_IRL" message
 *         boost::asio::write(
 *             socket,
 *             boost::asio::buffer("get_IRL\r"),
 *             getIRL_error
 *         );
 *         if(getIRL_error) {
 *             throw "Unable to start learning mode";
 *         }
 *         else {
 *             std::cout << "\nSocket Connected\nPress ENTER to stop listening...";
 *         }
 *         // start learning thread
 *         boost::asio::streambuf recieve_buf(2048);
 * 
 *         // std::thread socketCloseThread(
 *             // [&]() {
 *                 // boost::system::error_code write_error, shutdownError;
 *                 // if(socket.is_open()) {
 *                     // std::cin.get();
 *                     // boost::asio::write(
 *                         // socket,
 *                         // boost::asio::buffer("stop_IRL\r"),
 *                         // write_error
 *                     // );
 *                     // socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdownError);
 *                     // if(!shutdownError) {
 *                         // std::cout << "\nSocket Disconnected";
 *                     // }
 *                     // else {
 *                         // throw "Error while shutting socket down";
 *                     // }
 *                 // }     
 *             // }
 *         // );
 *         // socketCloseThread.detach();
 * 
 *         boost::system::error_code rw_error;
 *         if(socket.is_open()) {            
 *             boost::asio::read_until(
 *                 socket,
 *                 recieve_buf,
 *                 '\r',
 *                 // boost::asio::transfer_all(),
 *                 rw_error
 *             );
 *             if(rw_error) {
 *                 std::cout << "\nSocket read closed";
 *             }
 * 
 *             boost::asio::write(
 *                     socket,
 *                     boost::asio::buffer("stop_IRL\r"),
 *                     rw_error
 *                     );
 *             socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, rw_error);
 *             if(!rw_error) {
 *                 std::cout << "\nSocket Disconnected";
 *             }
 *             else {
 *                 throw "Error while shutting socket down";
 *             }
 *         }     
 * 
 *         // close socket on click
 *         // sleep(10);
 *         // if(socketCloseThread.joinable()) {
 *             // socketCloseThread.join();
 *             // socketCloseThread.~thread();
 *         // }
 *         if(socket.is_open())
 *             socket.close();
 *         return std::string(boost::asio::buffer_cast<const char*>(recieve_buf.data()));
 *     }
 *     catch(const char *ex) {
 *         std::cout << std::endl << "\n GC_IR: " << ex << std::endl;
 *         socket.~basic_stream_socket();
 *         return "";
 *     }
 * }
 */

std::string GC_IR::_learnIRCommand(std::string host_ip, int host_port)
{
    namespace ba = boost::asio;
    // boost IO service
    ba::io_context io_ctx;
    // asio socket
    ba::ip::tcp::socket socket(io_ctx);
    // buffer size limit (for receiving buffer)
    const static int BUFF_SIZE = 4096;

    // string to store the result in
    std::string result;

    try {
        // connect socket
        socket.connect(ba::ip::tcp::endpoint(ba::ip::address::from_string(host_ip), host_port));
        // system error handler
        boost::system::error_code getIRL_error;
        // send "get_IRL" message
        ba::write(socket, ba::buffer("get_IRL\r"), getIRL_error);

        if(getIRL_error)
        {
            throw std::runtime_error("Unable to start learning mode");
        }

        // I("{}():{} Socket Connected", __func__, __LINE__);

        // start learning
        ba::streambuf receive_buff(BUFF_SIZE);
        boost::system::error_code rw_error;
        if(socket.is_open()) 
        {
            ba::read_until(socket, receive_buff, '\r', rw_error);
            if(rw_error)
            {
                // Logger.E("{}() Failed to read from socket", __func__);
            }

            ba::write(socket, ba::buffer("stop_IRL\r"), rw_error);
        }     

        result = std::string(ba::buffer_cast<const char*>(receive_buff.data()));
    }
    /*
     * catch(const GwException& e)
     * {
     *     // E("{}():{} Exception: {} Code: {}", __func__, __LINE__, e.what(), std::to_string(e.getErrorCode()));
     * }
     */
    catch(const std::exception& e)
    {
        // E("{}():{} Unexpected exception while reading data from socket: {}", __func__, __LINE__, e.what());
    }

    try
    {
        if(socket.is_open())
        {
            boost::system::error_code shutdown_error;
            socket.shutdown(ba::ip::tcp::socket::shutdown_both, shutdown_error);
            if(shutdown_error)
            {
                // E("{}(): Failed to shutdown socket", __func__);
            }
            socket.close();
        }
    }
    catch(const std::exception& e)
    {
        // E("{}():{} Unexpected exception while closing socket: {}", __func__, __LINE__, e.what());
    }
    
    return result;
}

/*
 * std::vector<std::string> GC_IR::formatIRCommand(std::string IR_response) {
 *     std::vector<std::string> commands;
 *     std::string delim = "sendir";
 *     
 *     try {
 *         // Remove 'IR Learner Enabled'
 *         // IR_response.erase(0, std::string("IR Learner Enabled").length());
 *         if(IR_response.find("IR Learner Enabled") != std::string::npos)
 *             boost::erase_all(IR_response, "IR Learner Enabled");
 * 
 *         // Remove 'IR Learner Disabled'
 *         if(IR_response.find("IR Learner Disabled") != std::string::npos)
 *             boost::erase_all(IR_response, "IR Learner Disabled");
 *             // IR_response.erase(
 *             //     IR_response.find("IR Learner Disabled"),
 *             //     IR_response.length()
 *             // );
 * 
 *         // Segregate the commands into a vector
 *         while(!IR_response.empty()) {
 *             size_t pos = IR_response.find(delim);
 *             commands.push_back(
 *                 IR_response.substr(0, IR_response.find(delim, 1))
 *             );
 *             IR_response.erase(0, IR_response.find(delim, 1));
 *         }
 *     }
 *     catch(std::exception &e) {
 *         std::cout << std::endl << e.what() << std::endl;
 *     }
 *     return commands;
 * }
 */

std::string rawToPronto(std::string raw)
{
    std::string res;
    // This function expects the raw code to be in the format
    /*
        sendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23.....
        <cmd>,<channel:channel_num>,<0_or_1>,<frequency>,<repeat>,<offset>,<word1_in_dec>,<word2_in_dec>
    */

    static const int MIN_EXPECTED_SIZE = 7;
    // every pronto code which is learned begins with "0000"
    std::vector<std::string> inputVector;
    boost::algorithm::split(inputVector, raw, boost::is_any_of(","));

    if(inputVector.size() < MIN_EXPECTED_SIZE)
    {
        return res;
    }

    std::string frequencyInProntoTerms;
    try
    {
        int f = 1000000 / (std::stoi(inputVector[3]) * 0.241246);
        frequencyInProntoTerms = GwUtils::integerToHexString(f);
    }
    catch(std::exception &e)
    {
        return res;
    }

    std::vector<std::string> resVector = {"0000", frequencyInProntoTerms, "0000", ""};

    try
    {
        // Considering the preamble to always be "0000 <frequency_in_pronto_terms> 0000 <number_of_burst_pairs>"
        // http://www.remotecentral.com/features/irdisp2.htm
        // http://www.remotecentral.com/features/irdisp3.htm
        // http://www.remotecentral.com/features/irdisp4.htm
        // TODO: This will only work if burst there is no load in burst pair
        // since we are always considering it to be 0
        // refer to the above links for more info on the terminology

        // start pushing back elements in pairs of two (burst pairs) to the result
        for (auto itr = inputVector.begin() + MIN_EXPECTED_SIZE - 1; itr != inputVector.end(); itr++)
        {
            resVector.push_back(GwUtils::integerToHexString(std::stoi(*itr)));
            itr++;
            if(itr == inputVector.end())
            {
                throw std::runtime_error("Burst pair corrupted");
            }
            resVector.push_back(GwUtils::integerToHexString(std::stoi(*itr)));
        }

        int numBurstPairs = (resVector.size() - 4) / 2;
        resVector[3] = GwUtils::integerToHexString(numBurstPairs);
        res = boost::algorithm::join(resVector, " ");
    }
    // TODO: add logs
    catch(boost::bad_lexical_cast &e)
    {
        std::cout << e.what();
    }
    catch(std::runtime_error& e)
    {
        std::cout << __LINE__ << ": " << e.what();
    }
    catch(std::exception &e)
    {
        std::cout << __LINE__ << ": " << e.what();
    }

    return res;
}

std::string GC_IR::cleanseIRCommand(std::string IR_response)
{
    std::string ret;
    try
    {
        // Remove 'IR Learner Enabled'
        // IR_response.erase(0, std::string("IR Learner Enabled").length());
        if(IR_response.find("IR Learner Enabled") != std::string::npos)
            boost::erase_all(IR_response, "IR Learner Enabled");

        // Remove 'IR Learner Disabled'
        if(IR_response.find("IR Learner Disabled") != std::string::npos)
            boost::erase_all(IR_response, "IR Learner Disabled");

        boost::erase_all(IR_response, "\r");

        ret = IR_response;
    }
    catch (const std::exception& e)
    {

    }

    return ret;
}

int main(int argc, char **argv) {
    if(argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <ip>\n";
        return 1;
    }
    std::string s =
    "IR Learner Enabledsendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692\rsendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692sendir\r,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19, \
    23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19, \
    23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692\rIR Learner Disabled";

    // std::vector<std::string> v = GC_IR::formatIRCommand(s);
    std::string ss = rawToPronto(GC_IR::cleanseIRCommand(GC_IR::_learnIRCommand(argv[1])));
    std::cout << ss << "\n";
    /*
     * std::vector<std::string> v = GC_IR::formatIRCommand(ss);
     * try{
     *     for(std::string i : v) {
     *         std::cout << std::endl << i << std::endl;
     *     }
     * }
     * catch(std::exception &e) {
     *     std::cout << std::endl << e.what() << std::endl;
     * }
     */

    // std::cout << GC_IR::learnIRCommand("127.0.0.1", 3000);
    // v.~vector();

    return 0;
}
