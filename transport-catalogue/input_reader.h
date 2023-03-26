#pragma once
#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <unordered_map>
#include <set>

struct Stop
{
    std::string stop_name;
    double latitude = 0.0;
    double longitude = 0.0;
    std::string next_stops;
    std::unordered_map<std::string, std::uint32_t> dist_to_next;
};

struct Bus
{
    std::string bus_name;
    std::vector<std::string> route;
    bool is_circle = false;
    double r_length = 0.0;
    uint32_t true_length = 0;
    double curvature = 0.0;
};

struct InputResult
{
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;
    std::unordered_map<std::string, std::set<std::string>> buses_for_stops_;
};

void InputReader(std::istream& in, InputResult& input_results);

void RemoveBeginEndSpaces(std::string& str);
std::string Find_Name(std::string& str, char separator);

void AddStop(InputResult& r, std::string stop_sv);
void AddBus(InputResult& r, std::string bus_sv);

void AddNextStops(InputResult& r, Stop& stop);
void ComputeRealRouteLength(InputResult& r, Bus& bus);
