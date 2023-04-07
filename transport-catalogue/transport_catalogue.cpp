#include <iomanip>

#include "transport_catalogue.h"

namespace transport_catalogue {
	size_t StopPairHash::operator()(const std::pair<const Stop*, const Stop*>& p) const {
		return std::hash<const void*>()(p.first) * std::hash<const void*>()(p.second) * 1000000 + 43;
	}
	//Adds new stop
	void TransportCatalogue::AddStop(std::string stop_name, geo::Coordinates coordinates) {
		if (stop_index_.count(stop_name) == 0) {
			stop_and_buses_names_.push_back(stop_name);
			stops_.push_back({ stop_and_buses_names_.back(), coordinates });
			std::string_view view_name = stops_.back().name;
			stop_index_[view_name] = &stops_.back();
		}
	}
	//Sets distance between stops
	void TransportCatalogue::SetStopToStopDistances(const std::string stop_name, const std::string other_stop_name, double distance) {
		if (stop_to_stop_distances_.count({ stop_index_[other_stop_name], stop_index_[stop_name] }) == 0) {
			stop_to_stop_distances_[{ stop_index_[stop_name], stop_index_[other_stop_name] }] = distance;
			stop_to_stop_distances_[{ stop_index_[other_stop_name], stop_index_[stop_name] }] = distance;
		}
		else {
			stop_to_stop_distances_[{ stop_index_[stop_name], stop_index_[other_stop_name] }] = distance;
		}
	}

	//Adds new bus with collateral connections
	void TransportCatalogue::AddBus(std::string bus_name, std::vector<std::string>& vect_stops, bool is_circle) {
		if (buses_index_.count(bus_name) == 0) {
			std::vector<std::string> new_stops;
			if (!vect_stops.empty()) {
				new_stops = vect_stops;
				if (is_circle == false) {
					auto it = --vect_stops.end();
					--it;
					for (; it != vect_stops.begin(); --it) {
						new_stops.push_back(*it);
					}
					new_stops.push_back(vect_stops.front());
					vect_stops.clear();
					vect_stops = new_stops;
				}
			}
			std::vector<const Stop*> stops;
			for (std::string stop : vect_stops) {
				stops.push_back(stop_index_.at(stop));
				stops_with_routes_.push_back(stop_index_.at(stop)->coordinates);
			}
			stop_and_buses_names_.push_back(bus_name);
			buses_.push_back({ stop_and_buses_names_.back(), stops, is_circle });
			std::string_view bus_name_view = buses_.back().name;
			buses_index_[bus_name_view] = &buses_.back();
			for (std::string_view stop : new_stops) {
				std::string_view stop_name = stop_index_[stop]->name;
				buses_by_stop_[stop_name].insert(buses_.back().name);
			}
		}
	}

	//Returns all stops with their coords
	std::vector<geo::Coordinates> TransportCatalogue::GetStopsWithCoordinates() const {
		if (stops_with_routes_.size() > 0) {
			return stops_with_routes_;
		}
		return {};
	}

	//Returns bus info by its name
	transport_catalogue::BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
		if (buses_index_.count(name) > 0) {
			BusInfo bus_info{};
			const transport_catalogue::Bus* bus = buses_index_.at(name);
			std::vector<const transport_catalogue::Stop*> all_stops = bus->stops;
			auto stop1 = all_stops.begin();
			auto stop2 = all_stops.begin();
			++stop2;
			double road_distance = 0.0;
			double geo_road_length = 0.0;
			while (stop2 != all_stops.end()) {
				geo_road_length += ComputeDistance((*stop1)->coordinates, (*stop2)->coordinates);
				road_distance += GetStopToStopDistance(*stop1, *stop2);
				++stop1;
				++stop2;
			}

			double curvature = road_distance / geo_road_length;
			std::unordered_set<const transport_catalogue::Stop*> unique_stops(all_stops.begin(), all_stops.end());
			return bus_info = { bus->name, all_stops.size(), unique_stops.size(), static_cast<int>(road_distance), curvature, false };
		}
		static BusInfo empty_bus_info{};
		return empty_bus_info;
	}

	//Returns stop's info by its name
	transport_catalogue::StopInfo TransportCatalogue::GetStopInfo(std::string_view name) const {
		if (stop_index_.count(name) > 0) {
			std::vector<std::string_view> buses;
			auto stop_buses = GetBusesByStop(name);
			if (!stop_buses.empty()) {
				for (auto bus : stop_buses) {
					buses.push_back(bus);
				}
			}
			return { name, buses, false };
		}
		static StopInfo empty_stop_info{};
		return empty_stop_info;
	}

	//Returns route's info by its name
	std::vector<const Stop*> TransportCatalogue::GetBusRouteByName(std::string_view name) const {
		if (buses_index_.count(name) > 0) {
			return buses_index_.at(name)->stops;
		}
		static std::vector<const Stop*> empty_stops{};
		return empty_stops;
	}

	//Returns stop by its name
	Stop TransportCatalogue::GetStopByName(const std::string_view stop_name) const {
		if (stop_index_.count(stop_name) > 0) {
			return { stop_index_.at(stop_name)->name, stop_index_.at(stop_name)->coordinates };
		}
		static Stop empty_stop{};
		return empty_stop;
	}

	//Returns all buses that have such stop in their route
	std::set<std::string_view> TransportCatalogue::GetBusesByStop(std::string_view stop_name) const {
		if (buses_by_stop_.count(stop_name)) {
			return buses_by_stop_.at(stop_name);
		}
		static std::set<std::string_view> buses{};
		return buses;
	}

	//Returns the distance between two stops
	double TransportCatalogue::GetStopToStopDistance(const Stop* from, const Stop* to) const {
		return stop_to_stop_distances_.at({ from, to });
	}

	//Checks whether the stop exists or not
	bool TransportCatalogue::IsStopExist(std::string name) const {
		if (stop_index_.count(name) > 0) {
			return true;
		}
		return false;
	}

	//Returns all stops from the bus route
	std::vector<const Stop*> TransportCatalogue::GetStopsByBusName(std::string name) const {
		return buses_index_.at(name)->stops;
	}

	//Returns bus by its name
	const Bus* TransportCatalogue::GetBusByName(const std::string name) const {
		if (buses_index_.count(name) > 0) {
			return buses_index_.at(name);
		}
		return nullptr;
	}
}
