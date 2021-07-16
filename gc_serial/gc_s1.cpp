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
// Standard C++ Libraries
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <bits/stdc++.h>
#include <unistd.h>

namespace Zmote_serial {
    std::vector<std::string> split_str(std::string, std::string);
    void handleSerialResponse(std::string);
    std::string sendPocoRequest(std::string, std::string, std::string, std::string);
    std::string createRequestBody(std::string, std::string);
}

std::vector<std::string> Zmote_serial::split_str(std::string str, std::string delimiter) {
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
    }
    exit(EXIT_FAILURE);
}

void Zmote_serial::handleSerialResponse(std::string responseStr) {
    std::vector<std::string> response = Zmote_serial::split_str(responseStr, ",");

    if(response.at(0) != "completeir") {
        std::cout<<"\nzmote: Request Error\nResponse:\n "<<responseStr<<"\n";
        exit(EXIT_FAILURE);
    }
    else {
        std::cout<<"\nzmote: Response Complete!\nResponse:\n "<<responseStr<<"\n";
    }
}

std::string Zmote_serial::sendPocoRequest(std::string ip_addr, std::string zmote_uuid, std::string contentType, std::string requestBody) {
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
        if(str_stream.str().empty()) {
            std::cout<<"\nzmote_serial: No/Empty Response from zmote\n";
            exit(EXIT_FAILURE);
        }

        return str_stream.str();
    }
    catch(std::exception &e) {
        std::cout<<"\nzmote_serial: Poco Request Failed\n";
    }
    exit(EXIT_FAILURE);
}

std::string Zmote_serial::createRequestBody(std::string command, unsigned int baudrate, std::string flowcontrol, std::string parity) {
    try {
        if(command != "set_SERIAL" || command != "get_SERIAL") {
            std::cout<<"\nzmote_serial: Unknown Command `"<<command<<"`\n";
        }
        switch(baudrate) {
            case 115200:
            case 57600:
            case 38400:
            case 19200:
            case 14400:
            case 9600:
            case 4800:
            case 2400:
            case 1200:
                break;
            default:
                std::cout<<"\nzmote_serial: Incorrect baudrate\n";
                exit(EXIT_FAILURE);
        };
        if(flowcontrol != "FLOW_HARDWARE" || flowcontrol != "FLOW_NONE") {
            std::cout<<"\nzmote_serial: Incorrect flowcontrol\n";
            exit(EXIT_FAILURE);
        }
        if(parity != "PARITY_NO" || parity != "PARITY_EVEN" || parity != "PARITY_ODD") {
            std::cout<<"\nzmote_serial: Incorrect Parity Type\n";
            exit(EXIT_FAILURE);
        }
    }
    catch(std::exception &e) {
        throw e;
        exit(EXIT_FAILURE);
    }
    return command + ",1:1," + std::to_string(baudrate) + "," + parity;
}





int main(int argc, char *args[20]) {

    /* args[6] = [
        0: run exec
        1: ip_addr, 
        2: zmote_uuid, 
        3: option, 
        4: s_mod_frequency, 
        5: mark_space_timing
    ] */
    try {
        if(std::string(args[3]) == "-learn" && argc >= 3) {
            // Learning Mode
            //Zmote_serial::learningMode(args[1], args[2]);
        }
        else if(std::string(args[3]) == "-control" && argc == 5) {
            // Send IR Command
            Zmote_serial::handleSerialResponse(Zmote_serial::sendPocoRequest(args[1], args[2], "text/plain", Zmote_serial::createRequestBody(args[4], args[5])));
        }
        else if(argc < 3){
            std::exception();
        }
/* 
    // Function Calls for :
        // Send IR Command
        Zmote_serial::handleSerialResponse(Zmote_serial::sendPocoRequest(args[2], args[3], "text/plain", Zmote_serial::createRequestBody(args[4], args[5])));

        // Learning Mode :
        Zmote_serial::learningModeResponseHandler(Zmote_serial::sendPocoRequest(args[2], args[3], "text/plain", "get_IRL"), args[3]);
 */
    }
    catch(std::exception &e) {
        std::cout<<"zmote: too few arguments";
        std::cout<<"\nUsage:\n "<<args[0]<<" <ip-address> <uuid> [-options] [-params]";
        std::cout<<"\n [options]: \n  [-learn] = zmote learn mode\n";
        std::cout<<"\n  [-control] = send ir command \n\t [-params]:  <modulation_frequency> <mark_space_timing>\n";
        std::cout<<"\n  [-help] = Usage Help";
        std::cout<<std::endl;
    }
    return 0;
    
}