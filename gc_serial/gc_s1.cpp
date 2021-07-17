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
    std::string sendPocoRequest(std::string, std::string, std::string);
    std::string createRequestBody(std::string, unsigned int, std::string, std::string);
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
    
}

void Zmote_serial::handleSerialResponse(std::string responseStr) {
    if(responseStr.find("SERIAL") == std::string::npos) {
        std::cout<<"\nzmote_serial: SERIAL response error/not found"<<"\n";
    }
    else {
        std::cout<<"\nzmote_serial: Request Successful\n "<<responseStr<<std::endl;
    }
    return;
}

std::string Zmote_serial::sendPocoRequest(std::string ip_addr, std::string contentType, std::string requestBody) {
    try {
        ip_addr = "http://" + ip_addr + "/v2/";

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
            std::cout<<"\nzmote_serial: No/Empty Response from zmote";
        }
        else
            return str_stream.str();
    }
    catch(std::exception &e) {
        std::cout<<"\nzmote_serial: Poco Request Failed\n";
    }
    return "";
}

std::string Zmote_serial::createRequestBody(std::string command, unsigned int baudrate = 0, std::string flowcontrol = "", std::string parity = "") {
    try {
        bool error = false;
        if(command == "set_SERIAL"){
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
                    std::cout<<"\nzmote_serial: Incorrect baudrate "<<baudrate;
                    error = true;
                    
            };
            if(flowcontrol != "FLOW_HARDWARE" && flowcontrol != "FLOW_NONE") {
                std::cout<<"\nzmote_serial: Incorrect flowcontrol";
                error = true;
                
            }
            if(parity != "PARITY_NO" && parity != "PARITY_EVEN" && parity != "PARITY_ODD") {
                std::cout<<"\nzmote_serial: Incorrect Parity Type";
                error = true;
            }
            if(!error)
                return command + ",1:1," + std::to_string(baudrate) + "," + flowcontrol + "," + parity;
        }
        else if(command == "get_SERIAL"){
            return command + ",1:1";
        }
        else {
            std::cout<<"\nzmote_serial: Unknown Command `"<<command<<"`";
        }
    }
    catch(std::exception &e) {
        throw e;
    }
    return "";
}


int main(int argc, char *args[20]) {

    /* args[6] = [
        0: run exec
        1: ip_addr, 
        2: command, 
        3: baudrate, 
        4: flowcontrol, 
        5: parity
    ] */
    try {
        if(std::string(args[2]) == "set_SERIAL") {
            Zmote_serial::handleSerialResponse(Zmote_serial::sendPocoRequest(args[1], "text/plain", Zmote_serial::createRequestBody(args[2], std::stoi(args[3]), args[4], args[5])));
        }
        else if(std::string(args[2]) == "get_SERIAL")
            Zmote_serial::handleSerialResponse(Zmote_serial::sendPocoRequest(args[1], "text/plain", Zmote_serial::createRequestBody(args[2])));
        else {
            std::cout<<"zmote_serial: too few arguments";
            std::cout<<"\nUsage:\n "<<args[0]<<" <ip-address> <options> [-params]";
            std::cout<<"\n <options>: \n  get_SERIAL\n";
            std::cout<<"\n  set_SERIAL \n\t [-params]:  <baudrate> <flowcontrol> <parity>\n";
            std::cout<<"\n  help = Usage Help";
            std::cout<<std::endl;
        }
/* 
    // Function Calls for :
        // get_SERIAL
        Zmote_serial::handleSerialResponse(Zmote_serial::sendPocoRequest(args[1], "text/plain", Zmote_serial::createRequestBody("get_SERIAL")));

        // set_SERIAL
        Zmote_serial::handleSerialResponse(Zmote_serial::sendPocoRequest(args[1], "text/plain", Zmote_serial::createRequestBody("set_SERIAL", 115200, "FLOW_NONE", "PARITY_NO")));
 */
    }
    catch(std::exception &e) {

    }
    return 0;
    
}