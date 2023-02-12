#include "transport_catalogue.h"
#include <functional>

using namespace catalogue;

void RemoveBeginEndSpaces(std::string_view& str){
    while(str.front() == ' '){
        str.remove_prefix(1);
    }
    while(str.back() == ' '){
        str.remove_suffix(1);
    }
}
std::string_view Find_Name(std::string_view& sv, char separator){
    std::string_view name;
    std::uint64_t sep_pos;
    if(sv.find(separator) != sv.npos){ //Ищем признак окончания названия
        sep_pos = sv.find(separator);
    }
    else{
        sep_pos = sv.npos;
    }
    name = sv.substr(0, sep_pos);
    RemoveBeginEndSpaces(name); //Убираем начальные и конечные пробелы
    if(sep_pos != sv.npos){
        sv.remove_prefix(sep_pos + 1);
    }
    else{
        sv.remove_prefix(sv.size());
    }
    return name;
}
TransportCatalogue::TransportCatalogue(std::deque<std::string> q)
:queries_(std::move(q))
{
    for(const auto& i : queries_){
        if(i.substr(0, 4) == "Stop"){
            AddStop(i);
        }
        if(i.substr(0, 3) == "Bus"){
            AddBus(i);
        }
    }
    for(auto& i : stops_){
        AddNextStops(i.second);
    }
    for(auto& i : buses_){
        ComputeRealRouteLength(i.second);
    }
}

void TransportCatalogue::AddStop(std::string_view stop_sv){
    Stop stop;  
    stop_sv.remove_prefix(4);
    stop.stop_name = Find_Name(stop_sv, ':');
    std::string_view lat_ = Find_Name(stop_sv, ',');
    stop.latitude = std::stod({lat_.data(), lat_.size()});
    std::string_view lng_ = Find_Name(stop_sv, ',');
    stop.longitude = std::stod({lng_.data(), lng_.size()});
    stop.next_stops = stop_sv; //string_view с оставшейся информацией для последующей обработки
    stops_.insert({stop.stop_name, stop});
    buses_for_stops_.insert({stop.stop_name, {}});
}

void TransportCatalogue::AddNextStops(Stop& stop){
    while(!stop.next_stops.empty()){
        std::string_view distance = Find_Name(stop.next_stops, 'm');
        stop.next_stops.remove_prefix(stop.next_stops.find("to"));
        stop.next_stops.remove_prefix(2);
        std::string_view next_name = Find_Name(stop.next_stops, ',');
        if(stops_.count(next_name)){
            stop.dist_to_next.insert({stops_.at(next_name).stop_name, std::stoi({distance.data(), distance.size()})});
        }
    }
}

void TransportCatalogue::AddBus(std::string_view bus_sv){
    Bus bus;
    bus_sv.remove_prefix(3);
    bus.bus_name = Find_Name(bus_sv, ':');
    char sep = '-';
    if(bus_sv.find('>') != std::string_view::npos){
        bus.is_circle = true;
        sep = '>';
    }
    while(!bus_sv.empty()){
        auto stop_name = Find_Name(bus_sv, sep);
        if(stops_.count(stop_name)){
            Stop* stop_ptr = &stops_.at(stop_name);
            bus.route.push_back(stop_ptr);
            buses_for_stops_[stop_ptr->stop_name].insert(bus.bus_name);
        }
    }
    for(size_t i = 1; i < bus.route.size(); ++i){
        if(bus.route[i - 1] == bus.route[i]){
            continue;
        }
        else{
            bus.r_length += ComputeDistance({bus.route[i - 1]->latitude, bus.route[i - 1]->longitude},
                                            {bus.route[i]->latitude, bus.route[i]->longitude});
        }
    }
    if(!bus.is_circle){
        bus.r_length *= 2;
    }
    buses_.insert({bus.bus_name, bus});
}


void TransportCatalogue::ComputeRealRouteLength(Bus& bus){
    for(size_t i = 1; i < bus.route.size(); ++i){
        if(bus.route[i - 1]->dist_to_next.count(bus.route[i]->stop_name)){
            bus.true_length += bus.route[i - 1]->dist_to_next.at(bus.route[i]->stop_name);
        }
        else if(bus.route[i]->dist_to_next.count(bus.route[i - 1]->stop_name)){
            bus.true_length += bus.route[i]->dist_to_next.at(bus.route[i - 1]->stop_name);
        }
    }
    if(!bus.is_circle){
        for(size_t i = 1; i < bus.route.size(); ++i){
            if(bus.route[i]->dist_to_next.count(bus.route[i - 1]->stop_name)) {
                bus.true_length += bus.route[i]->dist_to_next.at(bus.route[i - 1]->stop_name);
            }
            else if(bus.route[i - 1]->dist_to_next.count(bus.route[i]->stop_name)) {
                bus.true_length += bus.route[i - 1]->dist_to_next.at(bus.route[i]->stop_name);
            }
        }
    }
    bus.curvature = bus.true_length / bus.r_length;
}

Stop TransportCatalogue::GetStop(std::string_view stop){
    return stops_.at(stop);
}

Bus TransportCatalogue::GetBus(std::string_view bus){
    return buses_.at(bus);
}

BusRoute TransportCatalogue::RouteInformation(std::string_view bus){
    RemoveBeginEndSpaces(bus);
    BusRoute route;
    if(buses_.count(bus)){
        route.is_found = true;
        std::set<const catalogue::Stop*> unique_stops(buses_.at(bus).route.begin(),buses_.at(bus).route.end() );
        route.bus_name = buses_.at(bus).bus_name;
        route.stops = (buses_.at(bus).is_circle) ? (buses_.at(bus).route.size()) : (buses_.at(bus).route.size() * 2 - 1);
        route.unique_stops = unique_stops.size();
        route.true_length = buses_.at(bus).true_length;
        route.curvature = buses_.at(bus).curvature;
    }
    else{
        route.bus_name = bus;
    }
    return route;
}
StopRoutes TransportCatalogue::StopInformation(std::string_view stop){
    RemoveBeginEndSpaces(stop);
    StopRoutes buses_for_stop;
    if(buses_for_stops_.count(stop)){
        buses_for_stop.is_found = true;
        buses_for_stop.stop_name = stop;
        buses_for_stop.routes = buses_for_stops_.at(stop);
    }
    else{
        buses_for_stop.stop_name = stop;
    }
    return buses_for_stop;
}
