
#include "transport_catalogue.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include "geo.h"
#include <functional>


using namespace std::literals;

static const Stop invalid_stop = { ""s, {} };	// unexisted stop
static const Bus invalid_bus = { ""s, {} };		// unexisted bus


static bool StopIsValid(const Stop& stop) {	
	return !stop.name.empty();
}

static bool BusIsValid(const Bus& bus) {
	return !bus.name.empty();
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

bool TransportCatalogue::AddBus(const BusNew& bus) {

	Bus bus_to_add = { bus.name, {} };
	for (const std::string& stop_name : bus.route) {
		if (const Stop& curr_stop = FindStop(stop_name); StopIsValid(curr_stop)) {
			bus_to_add.route.push_back(&curr_stop);
		}
		else return false;
	}
	buses_.push_back(bus_to_add);
	busname_to_bus_[buses_.back().name] = &buses_.back();
	// for Sop X command
	for (const auto& stop : bus_to_add.route) {
		stop_to_buses_[stop].push_back( &buses_.back() );
	}
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

int TransportCatalogue::GetUniqueStopsNum(const Bus& bus) const {
	std::vector<const Stop*> unique_stops(bus.route.begin(), bus.route.end());
	std::sort(unique_stops.begin(), unique_stops.end());
	unique_stops.erase(std::unique(unique_stops.begin(), unique_stops.end()), unique_stops.end());
	return static_cast<int>( unique_stops.size() );
}


std::optional<TransportCatalogue::BusInfo> TransportCatalogue::GetBusInfo(std::string bus_name) const {
	const Bus& bus = FindBus(bus_name);
	if (!BusIsValid(bus)) return std::nullopt;
	double factitial_length = CalculateRouteLengthViaLengths(bus);
	double geographical_length = CalculateRouteLengthViaCoordinates(bus);
	return BusInfo{ bus.route.size(),GetUniqueStopsNum(bus), factitial_length, factitial_length / geographical_length };
}


std::optional<std::vector<std::string_view>> TransportCatalogue::GetBusesForStop(const std::string& stop_name) const {
	std::vector<std::string_view> result;
	if (const Stop& stop = FindStop(stop_name); StopIsValid(stop)) {
		if (const auto it = stop_to_buses_.find(&stop); it != stop_to_buses_.end()) {
			for (const Bus* bus : it->second) {
				result.push_back(std::string_view(bus->name));
			}
			std::sort(result.begin(), result.end());
			result.erase(std::unique(result.begin(), result.end()), result.end());
		}
		else {
			return result;
		}
	}
	else {
		return std::nullopt;
	}
	return result;
}

void TransportCatalogue::AddStopsLength(std::string name_from, std::string name_to, double length) {
	const Stop& stop_from = FindStop(name_from);
	if (!StopIsValid(stop_from)) return;

	const Stop& stop_to = FindStop(name_to);
	if (!StopIsValid(stop_to)) return;

	stop_to_stop_distances_[ { &stop_from, &stop_to } ] = length;

	if (const auto it = stop_to_stop_distances_.find({ &stop_to, &stop_from }); it == stop_to_stop_distances_.end()) {
		stop_to_stop_distances_[{ &stop_to, &stop_from }] = length;
	}	
}



double TransportCatalogue::CalculateRouteLengthViaCoordinates(const Bus& bus) const {
	std::vector<double> distances(bus.route.size() - 1);
	std::transform(
		std::next(bus.route.begin()),
		bus.route.end(),
		bus.route.begin(),
		distances.begin(),
		[](const auto lhs, const auto rhs) {
			return ComputeDistance(lhs->coordinates, rhs->coordinates);
		}
	);
	double distance = std::reduce(
		distances.begin(),
		distances.end(),
		0.0,
		std::plus<>()
	);
	return distance;
}

double TransportCatalogue::CalculateRouteLengthViaLengths(const Bus& bus) const {
	std::vector<double> distances(bus.route.size() - 1);
	std::transform(
		std::next(bus.route.begin()),
		bus.route.end(),
		bus.route.begin(),
		distances.begin(),
		[this](const auto lhs, const auto rhs) {
			return this->GetStopsLength(*lhs, *rhs);
		}
	);
	double distance = std::reduce(
		distances.begin(),
		distances.end(),
		0.0,
		std::plus<>()
	);
	return distance;
}


double TransportCatalogue::GetStopsLength(const Stop& stop_from, const Stop& stop_to) const {
	//return stop_to_stop_distances_.at({ &stop_from, &stop_to });	
	return stop_to_stop_distances_.at({ &stop_from, &stop_to });

	
}







/*
std::optional<double> TransportCatalogue::GetDistanceBetweenStops(std::string name_from, std::string name_to) const {
	const Stop& stop_from = FindStop(name_from);
	if (!StopIsValid(stop_from)) return std::nullopt;

	const Stop& stop_to = FindStop(name_to);
	if (!StopIsValid(stop_to)) return  std::nullopt;
	
	if (const auto it = stop_to_stop_distances_.find({ &stop_from, &stop_to }); it != stop_to_stop_distances_.end()) {
		return (it->second);
	}
	else {
		return (ComputeDistance(stop_from.coordinates, stop_to.coordinates));
	}	
}

double TransportCatalogue::GetStopsDistance(const Stop& stop_from, const Stop& stop_to) const {
	if (const auto it = stop_to_stop_distances_.find({ &stop_from, &stop_to }); it != stop_to_stop_distances_.end()) {
		return (it->second);
	}
	else {
		return (ComputeDistance(stop_from.coordinates, stop_to.coordinates));
	}
}
*/

//double TransportCatalogue::ComputeStopsDistanceViaCoordinates(const Stop& stop_from, const Stop& stop_to) const {
//	return (ComputeDistance(stop_from.coordinates, stop_to.coordinates));
//}
//
//double TransportCatalogue::ComputeStopsDistanceViaLengths(const Stop& stop_from, const Stop& stop_to) const {
//	return stop_to_stop_distances_.at({ &stop_from, &stop_to });
//
//
//}