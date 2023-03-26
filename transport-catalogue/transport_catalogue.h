#pragma once
#include "geo.h"
#include "input_reader.h"
#include <unordered_map>
#include <string_view>
#include <string>
#include <deque>
#include <vector>
#include <functional>
#include <set>
#include <cstdint>

namespace catalogue
{
    struct BusRoute
    {
        std::string bus_name;
        size_t stops = 0;
        size_t unique_stops = 0;
        uint32_t true_length = 0;
        double curvature = 0.0;
        bool is_found = false;
    };

    struct StopRoutes
    {
        std::string stop_name;
        std::set<std::string> routes;
        bool is_found = false;
    };

 class TransportCatalogue {
    public:
        TransportCatalogue() = default;
        explicit TransportCatalogue(const InputResult& r);

        Stop GetStop(std::string stop);

        Bus GetBus(std::string bus);

        BusRoute RouteInformation(std::string bus);

        StopRoutes StopInformation(std::string stop);

    private:
        InputResult data_;
    };
}//namespace catalogue
