#include "stat_reader.h"
#include <iostream>

using namespace std::string_literals;
using namespace catalogue;

void Stat_Reader(std::istream& in, TransportCatalogue t) {
    int num;
    in >> num;
    in.ignore();
    for (int i = 0; i < num; ++i) {
        std::string query;
        std::getline(in, query);
        while (query[0] == ' ') {
            query.erase(0, 1);
        }
        if (query.substr(0, 3) == "Bus"s) {
            query.erase(0, 3);
            BusRoute r = t.RouteInformation(query);
            PrintBusInfo(std::cout, r);
        }
        if (query.substr(0, 4) == "Stop"s) {
            query.erase(0, 4);
            StopRoutes s = t.StopInformation(query);
            PrintStopInfo(std::cout, s);
        }
    }
}
void PrintBusInfo(std::ostream& out, BusRoute& route) {
    if (!route.is_found) {
        out << "Bus "s << route.bus_name << ": not found"s << std::endl;
    }
    else {
        out << "Bus "s << route.bus_name << ": " << route.stops << " stops on route, "s << route.unique_stops << " unique stops, "s << route.true_length << " route length, "s << route.curvature << " curvature"s << std::endl;
    }
}

void PrintStopInfo(std::ostream& out, const StopRoutes& stop) {
    if (!stop.is_found) {
        out << "Stop "s << stop.stop_name << ": not found"s << std::endl;
    }
    else if (stop.is_found && stop.routes.empty()) {
        out << "Stop "s << stop.stop_name << ": no buses"s << std::endl;
    }
    else {
        out << "Stop "s << stop.stop_name << ": buses"s;
        for (auto i : stop.routes) {
            out << " "s << i;
        }
        std::cout << std::endl;
    }
}
