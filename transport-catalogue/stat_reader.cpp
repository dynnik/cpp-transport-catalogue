#include "stat_reader.h"
#include <iostream>

using namespace std::string_literals;
using namespace catalogue;

void Stat_Reader(TransportCatalogue t){
    int num;
    std::cin >> num;
    std::cin.ignore();
    for(int i = 0; i < num; ++i){
        std::string query;
        std::getline(std::cin, query);
        while (query[0] == ' '){
            query.erase(0, 1);
        }
        if(query.substr(0, 3) == "Bus"s){
            query.erase(0, 3);
            BusRoute r = t.RouteInformation(query);
            PrintBusInfo(r);
        }
        if(query.substr(0, 4) == "Stop"s){
            query.erase(0, 4);
            StopRoutes s = t.StopInformation(query);
            PrintStopInfo(s);
        }
    }
}
void PrintBusInfo(BusRoute route){
    if(!route.is_found){
        std::cout << "Bus "s << route.bus_name << ": not found"s << std::endl;
    }
    else{
        std::cout << "Bus "s << route.bus_name << ": " << route.stops << " stops on route, "s << route.unique_stops << " unique stops, "s << route.true_length << " route length, "s << route.curvature << " curvature"s<<std::endl;
    }
}

void PrintStopInfo(const StopRoutes& stop){
    if(!stop.is_found){
        std::cout << "Stop "s << stop.stop_name << ": not found"s << std::endl;
    }
    else if(stop.is_found && stop.routes.empty()){
        std::cout << "Stop "s << stop.stop_name << ": no buses"s << std::endl;
    }
    else{
        std::cout << "Stop "s << stop.stop_name << ": buses"s;
        for(auto i : stop.routes){
            std::cout << " "s << i;
        }
        std::cout << std::endl;
    }
}
