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

std::vector<std::string> split_str(std::string str, std::string delimiter) {
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

void zmote_responseHandler(std::string responseStr) {
    std::vector<std::string> response = split_str(responseStr, ",");

    if(response.at(0) != "completeir") {
        std::cout<<"\nzmote: Request Error\nResponse:\n "<<responseStr<<"\n";
        exit(EXIT_FAILURE);
    }
    else {
        std::cout<<"\nzmote: Response Complete!\nResponse:\n "<<responseStr<<"\n";
    }
}

std::string zmote_sendPocoRequest(std::string ip_addr, std::string zmote_uuid, std::string contentType, std::string requestBody) {
    try {
        ip_addr = "http://" + ip_addr + "/v2/" + zmote_uuid;

        Poco::URI uri(ip_addr);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

        std::string path(uri.getPathAndQuery());
        if(path.empty()) {
            path = '/';
        }

        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, Poco::Net::HTTPMessage::HTTP_1_1);
        req.setContentLength(requestBody.length());
        req.setContentType(contentType);
        session.sendRequest(req) << requestBody;

        Poco::Net::HTTPResponse res;
        std::istream &is = session.receiveResponse(res);

        std::stringstream str_stream;
        Poco::StreamCopier::copyStream(is, str_stream);
        if(str_stream.str().empty()) {
            std::cout<<"\nzmote: No Response from zmote\n";
            exit(EXIT_FAILURE);
        }

        return str_stream.str();
    }
    catch(std::exception &e) {
        throw e;
    }
    std::cout<<"zmote: Poco Request Failed";
    exit(EXIT_FAILURE);
}

std::string zmote_sendIRCommand(std::string s_mod_frequency, std::string mark_space_timing) {
    int i_mod_freq;
    try {
        i_mod_freq = std::stoi(s_mod_frequency);
    }
    catch(std::exception &e) {
        throw e;
    }
    if(!(i_mod_freq >= 36000 && i_mod_freq <= 60000)) {
            std::cout<<"\nzmote: modulation frequency is out of range [36kHz, 60kHz]\n";
            exit(EXIT_FAILURE);
        }

    return "sendir,1:1,0," + s_mod_frequency + ",1,1," + mark_space_timing;
}

std::string zmote_learningMode(std::string pocoResponse, std::string zmote_uuid) {
    //pocoResponse = zmote_sendPocoRequest(ip_addr, zmote_uuid, "text/plain", "get_IRL");
    std::string delim = "IR Learner Enabled";

    try {
        pocoResponse = pocoResponse.erase(0, pocoResponse.find(delim) + delim.length());
        return "{zmote_learned: {'uuid': '" + zmote_uuid + "', 'ir_command': '" + pocoResponse + "'}}";
    }
    catch(std::exception &e) {
        std::cout<<"\nzmote: learning error";
    }
    exit(EXIT_FAILURE);
}

int main(int argc, char *args[]) {

    // args[] = [0:ip_addr, 1:zmote_uuid, 2:s_mod_frequency, 3:mark_space_timing]

    try {
        
    // Test Code :
        std::cout<<zmote_sendIRCommand(args[3], args[4]);
        zmote_responseHandler("completeir,1:1,<id>");
        zmote_responseHandler("busyir,1:1,<id>");
        zmote_responseHandler("error,1:1,low memory");
        std::cout<<zmote_learningMode("IR Learner Enabled\
sendir,1:1,0,37000,1,1,323,164,20,62,19,63,20,21,20,21,20,21,19,22,20,63,20,3692", "zmote011")<<"\n";

    // Function Calls for :
        // Send IR Command
        //zmote_responseHandler(zmote_sendPocoRequest(args[1], "text/plain", zmote_sendIRCommand(args[2], args[3])));

        // Learning Mode :
        //zmote_learningMode(zmote_sendPocoRequest(args[0], args[1], "text/plain", "get_IRL"), args[1]);
    }
    catch(std::exception &e) {
        std::cout<<"zmote: too few arguments";
        std::cout<<"\nUsage:\n "<<args[0]<<" <ip-address> <modulation_frequency> <mark_space_timing>\n";
    }
    return 0;
    
}