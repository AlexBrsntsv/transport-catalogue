
#include "transport_catalogue.h"
#include <algorithm>
#include <iostream>


using namespace std::literals;

static const Stop invalid_stop = { ""s, {90.0, 0.0} };	// north pole
static const Bus invalid_bus = { ""s, {} };				// unexisted bus




static bool StopIsValid(const Stop& stop) {	
	return stop != invalid_stop;
}

static bool BusIsValid(const Bus& bus) {
	return bus != invalid_bus;
}

void PrintBus(const Bus& bus) {
	std::cout << ( BusIsValid(bus) ? bus.name : "invalid bus"s) << ": "s;
	for (const auto stop_ref : bus.route) {
		std::cout << (stop_ref == nullptr ? "null"s : (*stop_ref).name) << " "s;
	}
	std::cout << std::endl;
}

void TransportCatalogue::AddStop(const Stop& stop) {
	stops_.push_back(stop);
	stopname_to_stop_[std::string_view(stops_.back().name)] = &stops_.back();
}


const Stop& TransportCatalogue::FindStop(std::string stop_name) const {		
	if (const auto it = stopname_to_stop_.find(stop_name); it == stopname_to_stop_.end()) {
		return invalid_stop;
	}
	else {
		if (it->second == nullptr) return invalid_stop;
		else return *(it->second);
	}
}

bool TransportCatalogue::AddBus(const std::string& bus_name, const std::vector<std::string>& stops_list) {

	Bus bus_to_add = { bus_name, {} };
	for (const std::string& stop_name : stops_list) {
		if (const Stop& curr_stop = FindStop(stop_name); StopIsValid(curr_stop)) {
			bus_to_add.route.push_back (&curr_stop);
		}
		else return false;
	}
	buses_.push_back(bus_to_add);
	busname_to_bus_[ buses_.back().name ] = &buses_.back();
	return true;

}

const Bus& TransportCatalogue::FindBus(std::string bus_name) const {
	if (const auto it = busname_to_bus_.find(bus_name); it == busname_to_bus_.end()) {
		return invalid_bus;
	}
	else {
		if (it->second == nullptr) return invalid_bus;
		else return *(it->second);
	}
}

std::optional<TransportCatalogue::BusInfo> TransportCatalogue::GetBusInfo(std::string bus_name) const {
	//const Bus& bus = FindBus(bus_name);
	//if (!BusIsValid(bus)) return std::nullopt;
	//std::vector<const Stop*> unique_stops(bus.route.begin(), bus.route.end());
	//unique_stops.erase( std::unique( unique_stops.begin(), unique_stops.end() ), unique_stops.end() );



	//return ( BusInfo{ bus.route.size(),unique_stops.size(), 0.0});
	return BusInfo{ 0,0, 0.0 };

}