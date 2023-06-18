// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "stat_reader.h"

using namespace std::literals;



static std::string BusInfoMessage(const std::string& bus_name, const TransportCatalogue::BusInfo& bus_info){
	
	int stops_on_route;
	int	unique_stops;
	double	route_length;
	std::tie<int, int, double>(stops_on_route, unique_stops, route_length) = bus_info;
	return 
		"Bus "s + bus_name + ": "s + 
		std::to_string(stops_on_route) + " stops on route, "s + 
		std::to_string(unique_stops) + " unique stops, " + 
		std::to_string(route_length) + " route length\n"s;
}

static std::string BusInfoErrorMessage(const std::string& bus_name) {
	return "Bus "s + bus_name + ": not found\n"s;
}


std::string ShowBusInfo(const TransportCatalogue& transport_catalogue, std::string bus_name) {
	if (const auto bi = transport_catalogue.GetBusInfo(bus_name); bi.has_value()) {
		return BusInfoMessage(bus_name, bi.value());
	}
	else {
		return BusInfoErrorMessage(bus_name);
	}
}