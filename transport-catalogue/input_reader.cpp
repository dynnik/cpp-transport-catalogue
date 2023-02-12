#include "input_reader.h"
#include <iostream>

using namespace std::string_literals;

std::deque<std::string> InputReader() {
    int req_num;
    std::deque<std::string> input_requests;
    std::cin >> req_num;
    std::cin.ignore();
    
    for(int i = 0; i < req_num; ++i)
    {
        std::string request;
        std::getline(std::cin, request);
        while (request[0] == ' ')
        {
            request.erase(0, 1);
        }
        if(request.substr(0, 4) == "Stop"s)
        {
            input_requests.push_front(request);
        }
        else if(request.substr(0, 3) == "Bus"s)
        {
            input_requests.push_back(request);
        }
    }
    return input_requests;
}
