// Poco Libraries
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Element.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
// RapidJSON Libraries
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
// Boost C++ Libraries
#include <boost/algorithm/string.hpp>
// Standard C++ Libraries
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <bits/stdc++.h>
#include <unistd.h>
#include <thread>

std::string learnedValue;

namespace Zmote_IR {

    std::string handleSendIRResponse(std::string);
    std::string sendPocoRequest(std::string, std::string, std::string, std::string);
    std::string createIRCommand(std::string, std::string);
    std::string learningModeResponseHandler(std::string, std::string);
    void learningMode(std::string, std::string);
    int startLearnerMode(std::string, std::string);
}

/* std::vector<std::string> Zmote_IR::split_str(std::string str, std::string delimiter) {
    std::vector<std::string> split;
    size_t pos = 0;
    try {
        while((pos = str.find(delimiter)) != std::string::npos) {
            split.push_back(str.substr(0, pos));
            str.erase(0, pos + delimiter.length());
        }
        return split;
    }
    catch(std::exception &e) {
        std::cout<<"split_str: Unable to split at delimiter "<<delimiter;
        throw e;
    }
} */

std::string Zmote_IR::handleSendIRResponse(std::string responseStr) {
    std::vector<std::string> response;
    try {
        boost::split(response, responseStr, boost::is_any_of(","));
        if(response.at(0) != "completeir") {
            throw "zmote: Request Error";
        }
        else {
            std::cout<<"\nzmote: Response Complete!\nResponse:\n "<<responseStr<<"\n";
            return responseStr;
        }
    }
    catch(const char *ex) {
        std::cout << std::endl << ex << std::endl;
        return "";
    }
}

std::string Zmote_IR::sendPocoRequest(std::string ip_addr, std::string zmote_uuid, std::string contentType, std::string requestBody) {
    try {
        ip_addr = "http://" + ip_addr + "/v2/" + zmote_uuid;

        Poco::URI uri(ip_addr);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

        std::string path(uri.getPathAndQuery());
        if(path.empty()) {
            path = '/';
        }

        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
        req.setContentLength(requestBody.length());
        req.setContentType(contentType);
        session.sendRequest(req) << requestBody;

        Poco::Net::HTTPResponse res;
        std::istream &is = session.receiveResponse(res);

        std::stringstream str_stream;
        Poco::StreamCopier::copyStream(is, str_stream);
        if(str_stream.str().empty())
            std::cout<<"zmote: No/Empty Response from zmote";
        else
            return str_stream.str();
    }
    catch(std::exception &e) {
        throw e;
        return "";
    }
}

std::string Zmote_IR::createIRCommand(std::string s_mod_frequency, std::string mark_space_timing) {
    int i_mod_freq;
    try {
        i_mod_freq = std::stoi(s_mod_frequency);
        // std::cout<<mark_space_timing;
        if(!(i_mod_freq >= 36000 && i_mod_freq <= 60000)) {
            throw "zmote: modulation frequency is out of range [36kHz, 60kHz]";
        }
        else
            return "sendir,1:1,0," + s_mod_frequency + ",1,1," + mark_space_timing;
    }
    catch(const char *ex) {
        std::cout << std::endl << ex << std::endl;
        return "";
    }
}

std::string Zmote_IR::learningModeResponseHandler(std::string pocoResponse, std::string zmote_uuid) {
    std::string delim = "IR Learner Enabled";
    if(pocoResponse.find("Disabled") != std::string::npos) {
        std::cout<<"\n"<<pocoResponse<<"\n";
        return "";
    }
    else {
        try {
            pocoResponse = pocoResponse.erase(0, pocoResponse.find(delim) + delim.length());
            return "{Zmote_IR::learned: {'uuid': '" + zmote_uuid + "', 'ir_command': '" + pocoResponse + "'}}";
        }
        catch(std::exception &e) {
            std::cout<<"\nzmote: learning error";
            return "";
        }
    }
}

int Zmote_IR::startLearnerMode(std::string ip_addr, std::string zmote_uuid) {
    if( !(learnedValue = Zmote_IR::learningModeResponseHandler(Zmote_IR::sendPocoRequest(ip_addr, zmote_uuid, "text/plain", "get_IRL"), zmote_uuid)).empty() ) {
        std::cout << "\nResponse: " << learnedValue << std::endl;
        return 0; // success
    }
    return -1; // error / empty response
}

void Zmote_IR::learningMode(std::string ip_addr, std::string zmote_uuid) {

    // Create learning mode thread
    std::thread startLearning((Zmote_IR::startLearnerMode), ip_addr, zmote_uuid);
    startLearning.detach(); // Detach learning mode thread
    std::cout << std::this_thread::get_id();
    // Listen to key press on 'main' thread
    std::cout<<"\nPress `return` to exit learner mode\n";
    std::cin.get();
    // Join learning mode thread to main thread if key pressed
    // startLearning.join();
    if((learnedValue).empty()) {
        startLearning.~thread(); // Release thread's memory
        Zmote_IR::learningModeResponseHandler(Zmote_IR::sendPocoRequest(ip_addr, zmote_uuid, "text/plain", "stop_IRL"), zmote_uuid);
    }
    // std::cout << Zmote_IR::learnedValue;
    return;
}

int main(int argc, char **args) {

    /* args[6] = [
        0: run exec
        1: ip_addr, 
        2: zmote_uuid, 
        3: option, (learn/control mode)
        4: s_mod_frequency, 
        5: mark_space_timing
    ] */
    try {
        if(std::string(args[3]) == "-learn" && argc >= 3) {
            // Learning Mode
            Zmote_IR::learningMode(args[1], args[2]);
        }
        else if(std::string(args[3]) == "-control") {
            // Send IR Command
            Zmote_IR::handleSendIRResponse(Zmote_IR::sendPocoRequest(args[1], args[2], "text/plain", Zmote_IR::createIRCommand(args[4], args[5])));
        }
        else{
            std::cout<<"zmote: too few arguments";
            std::cout<<"\nUsage:\n "<<args[0]<<" <ip-address> <uuid> [-options] [-params]";
            std::cout<<"\n [options]: \n  [-learn] = zmote learn mode\n";
            std::cout<<"\n  [-control] = send ir command \n\t [-params]:  <modulation_frequency> <mark_space_timing>\n";
            std::cout<<"\n  [-help] = Usage Help";
            std::cout<<std::endl;
        }
/* 
    // Function Calls for :
        // Send IR Command
        Zmote_IR::handleSendIRResponse(Zmote_IR::sendPocoRequest(args[2], args[3], "text/plain", Zmote_IR::createIRCommand(args[4], args[5])));

        // Learning Mode :
        Zmote_IR::learningModeResponseHandler(Zmote_IR::sendPocoRequest(args[2], args[3], "text/plain", "get_IRL"), args[3]);
 */
    }
    catch(std::exception &e) {
    }
    return 0;
    
}