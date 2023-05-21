#pragma once
#include <iostream>
#include "request_handler.h"
#include "json.h"
#include "json_builder.h"

#include <set>
using namespace std::literals;

namespace json_reader {

    //ñîçäà¸ì êëàññ json_reader.

    class Json_Reader {
    public:
        Json_Reader(std::istream& input);

        const json::Array& GetBaseRequests() const;
        const json::Dict& GetRenderSettings() const;
        const json::Dict& GetRoutingSettings() const;
        const json::Array& GetStatRequests() const;

        void AddRouteCoordinates(geo::Coordinates coordinates);
        const std::vector<geo::Coordinates>& GetRoutesCoordinate() const;

        void AddStopToStopRequest(const json::Node& node);
        void AddBusRequest(const json::Node& node);
        const Requests& GetRequests() const;

        void AddBusName(const std::string& bus_name);
        const std::set<std::string>& GetBusNames() const;

    private:

        
        Requests requests_;
        json::Array responses_;
        json::Document doc_;
        std::vector<geo::Coordinates> route_coordinates_;
        std::set<std::string> buses_names_;
    };


    void FillStopsByRequestBody(transport_catalogue::TransportCatalogue& transport_catalogue, const json::Node& request_body, Requests& requests);
    void FillStopToStopDistances(transport_catalogue::TransportCatalogue& transport_catalogue, const json::Node& stop_to_stop_distance);
    void FillRoutesByRequestBody(transport_catalogue::TransportCatalogue& transport_catalogue, const json::Node& add_buses_request);

    void GenerateResponse(request_handler::RequestHandler& request_handler, transport::router::TransportRouter& router, const json::Node& request_body, json::Array& responses);

    void AddStopInfoResponse(request_handler::RequestHandler& request_handler, const json::Node& request_body, json::Array& responses);
    void AddBusInfoResponse(request_handler::RequestHandler& request_handler, const json::Node& request_body, json::Array& responses);
    void AddMapInfoResponse(request_handler::RequestHandler& request_handler, const json::Node& request_body, json::Array& responses);
    void AddRouteInfoResponse(request_handler::RequestHandler& request_handler, const transport::router::TransportRouter& router, const json::Node& request_body, json::Array& responses);

    void SequentialRequestProcessing(transport_catalogue::TransportCatalogue& transport_catalogue, MapRenderer& map_render, std::istream& input, std::ostream& output, request_handler::RequestHandler& request_handler);
}
