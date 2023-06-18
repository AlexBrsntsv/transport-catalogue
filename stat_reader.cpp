// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "stat_reader.h"
#include <math.h>

using namespace std::literals;

static std::string BusInfoMessage(const std::string& bus_name, const TransportCatalogue::BusInfo& bus_info){
	
	int stops_on_route;
	int	unique_stops;
	double	route_length;
	std::tie<int, int, double>(stops_on_route, unique_stops, route_length) = bus_info;
	route_length = std::round(route_length * 100) / 100;
	return 
		"Bus "s + bus_name + ": "s + 
		std::to_string(stops_on_route) + " stops on route, "s + 
		std::to_string(unique_stops) + " unique stops, " + 
		std::to_string(route_length) + " route length"s;
}

static std::string BusInfoErrorMessage(const std::string& bus_name) {
	return "Bus "s + bus_name + ": not found"s;
}


std::string ShowBusInfo(const TransportCatalogue& transport_catalogue, const std::string& bus_name) {
	if (const auto opt_bus_info = transport_catalogue.GetBusInfo(bus_name); opt_bus_info.has_value()) {
		return BusInfoMessage(bus_name, opt_bus_info.value());
	}
	else {
		return BusInfoErrorMessage(bus_name);
	}
}


std::string StopInfoErrorMessage(const std::string& stop_name) {
	return "Stop "s + stop_name + ": not found"s;
}

std::string StopInfoEmptyMessage(const std::string& stop_name) {
	return "Stop "s + stop_name + ": no buses"s;
}

std::string StopInfoMessage(const std::string& stop_name, std::vector<std::string_view> buses) {
	std::string result = "Stop "s + stop_name + ": buses"s;
	for (std::string_view bus : buses) {
		result += ' ';
		result += std::string(bus);
	}
	return result;
}

std::string ShowStopInfo(const TransportCatalogue& transport_catalogue, const std::string& stop_name) {
	if (const auto opt_stop_info = transport_catalogue.GetBusesForStop(stop_name); opt_stop_info.has_value()) {
		if (opt_stop_info.value().empty()) return StopInfoEmptyMessage(stop_name);
		else return StopInfoMessage(stop_name, opt_stop_info.value());
	}
	else {
		return StopInfoErrorMessage(stop_name);
	}
}


std::string ShowInfo(const TransportCatalogue& tc, const Query& q) {
	if (q.type == QueryType::BusInfo) {
		return ( ShowBusInfo(tc, q.bus_name_info) );
	}
	else if (q.type == QueryType::StopInfo) {
		return (ShowStopInfo(tc, q.stop_name_info));
	}
	else {
		return "error"s;
	}
}