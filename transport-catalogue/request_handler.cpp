#include "request_handler.h"

#include <utility>
#include <sstream>
#include <unordered_set>

using namespace std;
using namespace transport;
using namespace domain;

RequestHandler::RequestHandler(const transport::Catalogue& catalogue,
    const transport::Router& router, const renderer::MapRenderer& renderer)
    : db_(catalogue)
    , router_(router)
    , renderer_(renderer) {}

void RequestHandler::JsonStatRequests(const json::Node& json_input, std::ostream& output)
{
    const json::Array& arr = json_input.AsArray();
    json::Array output_array;
    output_array.reserve(arr.size());
    for (auto& request_node : arr)
    {
        const json::Dict& request_map = request_node.AsDict();
        const string& type = request_map.at("type"s).AsString();
        if (type == "Stop"s) 
        {
            output_array.push_back(FindStopRequestProcessing(request_map));
            continue;
        }
        if (type == "Bus"s) 
        {
            output_array.push_back(FindBusRequestProcessing(request_map));
            continue;
        }
        if (type == "Map"s)
        {
            output_array.push_back(BuildMapRequestProcessing(request_map));
            continue;
        }
        if (type == "Route"s)
        {
            output_array.push_back(BuildRouteRequestProcessing(request_map));
            continue;
        }
    }
    json::Print(json::Document(json::Node(move(output_array))), output);
}

svg::Document RequestHandler::RenderMap() const
{
    return renderer_.GetSvgDocument(db_.GetSortedAllBuses());
}

json::Node RequestHandler::FindStopRequestProcessing(const json::Dict& request_map)
{
    int id = request_map.at("id"s).AsInt();
    const string& name = request_map.at("name"s).AsString();
    if (const Stop* stop = db_.FindStop(name))
    {
        json::Array buses_array;
        const auto& buses_on_stop = db_.GetBusesOnStop(stop->name);
        buses_array.reserve(buses_on_stop.size());
        for (auto& [bus_name, bus] : buses_on_stop)
        {
            buses_array.push_back(bus->name);
        }

        return json::Builder{}.StartDict()
            .Key("buses"s).Value(move(buses_array))
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }
    else {
        return json::Builder{}.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }
}

json::Node RequestHandler::FindBusRequestProcessing(const json::Dict& request_map)
{
    int id = request_map.at("id"s).AsInt();
    const string& name = request_map.at("name"s).AsString();
    if (const Bus* bus = db_.FindBus(name))
    {
        int stops_count = bus->stops.size();
        int distance = 0;
        double straight_distance = 0.0;
        for (int i = 1; i < stops_count; ++i)
        {
            distance += bus->stops[i - 1]->GetDistance(bus->stops[i]);
            straight_distance += geo::ComputeDistance(bus->stops[i - 1]->coordinates, bus->stops[i]->coordinates);
        }
        double curvature = distance / straight_distance;
        unordered_set<string> unique_stops_set;
        for (transport::Stop* s : bus->stops)
        {
            unique_stops_set.emplace(s->name);
        }
        int unique_stops = unique_stops_set.size();
        return json::Builder{}.StartDict()
            .Key("route_length"s).Value(distance)
            .Key("unique_stop_count"s).Value(unique_stops)
            .Key("stop_count"s).Value(stops_count)
            .Key("curvature"s).Value(curvature)
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }
    else {
        return json::Builder{}.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }
}

json::Node RequestHandler::BuildMapRequestProcessing(const json::Dict& request_map)
{
    int id = request_map.at("id"s).AsInt();
    svg::Document map = RenderMap();
    ostringstream strm;
    map.Render(strm);

    return json::Builder{}.StartDict()
        .Key("map"s).Value(strm.str())
        .Key("request_id"s).Value(id)
        .EndDict().Build();
}

json::Node RequestHandler::BuildRouteRequestProcessing(const json::Dict& request_map)
{
    int id = request_map.at("id"s).AsInt();
    const string& name_from = request_map.at("from"s).AsString();
    const string& name_to = request_map.at("to"s).AsString();
    if (const Stop* stop_from = db_.FindStop(name_from)) {
        if (const Stop* stop_to = db_.FindStop(name_to)) {
            if (auto ri = router_.GetRouteInfo(stop_from, stop_to)) {
                auto [weight, edges] = ri.value();
                return json::Builder{}.StartDict()
                    .Key("items"s).Value(router_.GetEdgesItems(edges))
                    .Key("total_time"s).Value(weight)
                    .Key("request_id"s).Value(id)
                    .EndDict().Build(); 
            }
        }
    }
    return json::Builder{}.StartDict()
        .Key("error_message"s).Value("not found"s)
        .Key("request_id"s).Value(id)
        .EndDict().Build();
}
