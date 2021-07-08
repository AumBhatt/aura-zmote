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

std::string zmote_sendPocoRequest(std::string ip_addr, Poco::Net::HTTPRequest requestMethod , std::string contentType, std::string requestBody) {
    try {
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

int main(int argc, char *args[]) {
    try {
        std::cout<<zmote_sendIRCommand(args[2], args[3]);
    }
    catch(std::exception &e) {
        std::cout<<"zmote: too few arguments";
        std::cout<<"\nUsage:\n "<<args[0]<<" <ip-address> <modulation_frequency> <mark_space_timing>\n";
    }
    return 0;
}