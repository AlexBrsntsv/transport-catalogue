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