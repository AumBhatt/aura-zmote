#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
int main() {

    std::stringstream stream;
    std::string gc = "347,173,22,65,22,22,22,65,22,65,22,22,22,22,22,65,22,22,22,22,22,65,22,22,22,22,22,65,22,65,22,22,22,65,22,65,22,22,22,22,22,65,22,65,22,65,22,65,22,65,22,22,22,65,22,65,22,22,22,22,22,22,22,22,22,22,22,65,22,65,22,65,22,22,22,22,22,65,22,22,22,22,22,22,22,22,22,22,22,65,22,65,22,22,22,65,22,65,22,487";
    std::vector<std::string> v;
    boost::split(v, gc, boost::is_any_of(","));
    std::cout << std::endl;
    for(std::string i : v){
        std::stringstream hexa;
        hexa << std::setfill ('0') << std::setw(sizeof(int)) << std::hex << std::stoi(i);
        std::cout << hexa.str() << " ";
    }
    std::cout << std::endl;

    return 0;
}