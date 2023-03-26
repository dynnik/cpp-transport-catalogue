#include "transport_catalogue.h"
#include <functional>

using namespace catalogue;


TransportCatalogue::TransportCatalogue(const InputResult& r)
{
    /*for (const auto& i : queries_) {
        if (i.substr(0, 4) == "Stop") {
            AddStop(i);
        }
        if (i.substr(0, 3) == "Bus") {
            AddBus(i);
        }
    }*/
    data_.stops_ = r.stops_;
    data_.buses_ = r.buses_;
    data_.buses_for_stops_ = r.buses_for_stops_;

    /*for (auto& i : data_.stops_) {
        AddNextStops(i.second);
    }
    for (auto& i : data_.buses_) {
        ComputeRealRouteLength(i.second);
    }*/
}

Stop TransportCatalogue::GetStop(std::string stop) {
    return data_.stops_.at(stop);
}

Bus TransportCatalogue::GetBus(std::string bus) {
    return data_.buses_.at(bus);
}

BusRoute TransportCatalogue::RouteInformation(std::string bus) {
    RemoveBeginEndSpaces(bus);
    BusRoute route;
    if (data_.buses_.count(bus)) {
        route.is_found = true;
        std::set<std::string> unique_stops(data_.buses_.at(bus).route.begin(), data_.buses_.at(bus).route.end());
        route.bus_name = data_.buses_.at(bus).bus_name;
        route.stops = (data_.buses_.at(bus).is_circle) ? (data_.buses_.at(bus).route.size()) : (data_.buses_.at(bus).route.size() * 2 - 1);
        route.unique_stops = unique_stops.size();
        route.true_length = data_.buses_.at(bus).true_length;
        route.curvature = data_.buses_.at(bus).curvature;
    }
    else {
        route.bus_name = bus;
    }
    return route;
}
StopRoutes TransportCatalogue::StopInformation(std::string stop) {
    RemoveBeginEndSpaces(stop);
    StopRoutes buses_for_stop;
    if (data_.buses_for_stops_.count(stop)) {
        buses_for_stop.is_found = true;
        buses_for_stop.stop_name = stop;
        buses_for_stop.routes = data_.buses_for_stops_.at(stop);
    }
    else {
        buses_for_stop.stop_name = stop;
    }
    return buses_for_stop;
}
