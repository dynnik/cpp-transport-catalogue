#include "input_reader.h"
#include "geo.h"
#include <iostream>

using namespace std::string_literals;

void InputReader(std::istream& in, InputResult& input_results) {
    
    int req_num;
    std::deque<std::string> input_;
    in >> req_num;
    in.ignore();
    for (int i = 0; i < req_num; ++i)
    {
        std::string request;
        std::getline(in, request);
        while (request[0] == ' ')
        {
            request.erase(0, 1);
        }
        if (request.substr(0, 4) == "Stop"s)
        {
            input_.push_front(request);
        }
        else if (request.substr(0, 3) == "Bus"s)
        {
            input_.push_back(request);
        }
    }
    for (const auto& i : input_) {
        if (i.substr(0, 4) == "Stop") {
            AddStop(input_results, i);
        }
        if (i.substr(0, 3) == "Bus") {
            AddBus(input_results, i);
        }
    }
    for (auto& i : input_results.stops_) {
        AddNextStops(input_results, i.second);
    }
    for (auto& i : input_results.buses_) {
        ComputeRealRouteLength(input_results, i.second);
    }
}

void RemoveBeginEndSpaces(std::string& str) {
    while (str.front() == ' ') {
        str.erase(0, 1);
    }
    while (str.back() == ' ') {
        str.erase(str.size()-1, 1); //??
    }
}

std::string Find_Name(std::string& sv, char separator) {
    std::string name;
    std::uint64_t sep_pos;
    if (sv.find(separator) != sv.npos) { //Ищем признак окончания названия
        sep_pos = sv.find(separator);
    }
    else {
        sep_pos = sv.npos;
    }
    name = sv.substr(0, sep_pos);
    RemoveBeginEndSpaces(name); //Убираем начальные и конечные пробелы
    if (sep_pos != sv.npos) {
        sv.erase(0, sep_pos + 1);
    }
    else {
        sv.erase(0, sv.size());
    }
    return name;
}

void AddStop(InputResult& r, std::string stop_sv) {
    Stop stop;
    stop_sv.erase(0, 4);
    stop.stop_name = Find_Name(stop_sv, ':');
    std::string lat_ = Find_Name(stop_sv, ',');
    stop.latitude = std::stod({ lat_.data(), lat_.size() });
    std::string lng_ = Find_Name(stop_sv, ',');
    stop.longitude = std::stod({ lng_.data(), lng_.size() });
    stop.next_stops = stop_sv; //string_view с оставшейся информацией для последующей обработки
    r.stops_.insert({ stop.stop_name, stop });
    r.buses_for_stops_.insert({ stop.stop_name, {} });
}

void AddBus(InputResult& r, std::string bus_sv) {
    Bus bus;
    bus_sv.erase(0, 3);
    bus.bus_name = Find_Name(bus_sv, ':');
    char sep = '-';
    if (bus_sv.find('>') != std::string_view::npos) {
        bus.is_circle = true;
        sep = '>';
    }
    while (!bus_sv.empty()) 
    {
        std::string stop_name = std::string(Find_Name(bus_sv, sep));
        if (r.stops_.count(stop_name)) { //
            bus.route.push_back(stop_name); // 
            r.buses_for_stops_[stop_name].insert(bus.bus_name); //
        }
    }
    for (size_t i = 1; i < bus.route.size(); ++i) {
        if (bus.route[i - 1] == bus.route[i]) {
            continue;
        }
        else {
            bus.r_length += ComputeDistance({ r.stops_[bus.route[i - 1]].latitude, r.stops_[bus.route[i - 1]].longitude },
                { r.stops_[bus.route[i]].latitude, r.stops_[bus.route[i]].longitude });
        }
    }
    if (!bus.is_circle) {
        bus.r_length *= 2;
    }
    r.buses_.insert({ bus.bus_name, bus });
}

void AddNextStops(InputResult& r, Stop& stop) {
    while (!stop.next_stops.empty()) {
        std::string distance = Find_Name(stop.next_stops, 'm');
        stop.next_stops.erase(0, stop.next_stops.find("to"));
        stop.next_stops.erase(0, 2);
        std::string next_name = Find_Name(stop.next_stops, ',');
        if (r.stops_.count(next_name)) {
            stop.dist_to_next.insert({ r.stops_.at(next_name).stop_name, std::stoi({distance.data(), distance.size()}) });
        }
    }
}

void ComputeRealRouteLength(InputResult& r, Bus& bus) {
    for (size_t i = 1; i < bus.route.size(); ++i) {
        if (r.stops_[bus.route[i - 1]].dist_to_next.count(bus.route[i])) {
            bus.true_length += r.stops_[bus.route[i - 1]].dist_to_next.at(bus.route[i]);
        }
        else if (r.stops_[bus.route[i]].dist_to_next.count(bus.route[i - 1])) {
            bus.true_length += r.stops_[bus.route[i]].dist_to_next.at(bus.route[i - 1]);
        }
    }
    if (!bus.is_circle) {
        for (size_t i = 1; i < bus.route.size(); ++i) {
            if (r.stops_[bus.route[i]].dist_to_next.count(bus.route[i - 1])) {
                bus.true_length += r.stops_[bus.route[i]].dist_to_next.at(bus.route[i - 1]);
            }
            else if (r.stops_[bus.route[i - 1]].dist_to_next.count(bus.route[i])) {
                bus.true_length += r.stops_[bus.route[i - 1]].dist_to_next.at(bus.route[i]);
            }
        }
    }
    bus.curvature = bus.true_length / bus.r_length;
}
