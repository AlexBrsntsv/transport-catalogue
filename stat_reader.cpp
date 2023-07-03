// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "stat_reader.h"
#include <math.h>



using namespace std::literals;

namespace transport {

namespace reader {

namespace statistics {

namespace {

static std::string BusInfoMessage(const std::string& bus_name, const transport::catalogue::BusInfo& bus_info) {
		
	double route_length = std::round(bus_info.route_length * 100) / 100;
	return
		"Bus "s + bus_name + ": "s +
		std::to_string(bus_info.stops_on_route) + " stops on route, "s +
		std::to_string(bus_info.unique_stops) + " unique stops, " +
		std::to_string(static_cast<int>(route_length)) + " route length, "s +
		std::to_string(bus_info.curvature) + " curvature"s;
}


static std::string BusInfoErrorMessage(const std::string& bus_name) {
	return "Bus "s + bus_name + ": not found"s;
}


std::string ShowBusInfo(const transport::catalogue::TransportCatalogue& transport_catalogue, const std::string& bus_name) {
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

std::string ShowStopInfo(const transport::catalogue::TransportCatalogue& transport_catalogue, const std::string& stop_name) {
	if (const auto opt_stop_info = transport_catalogue.GetBusesForStop(stop_name); opt_stop_info.has_value()) {
		if (opt_stop_info.value().empty()) return StopInfoEmptyMessage(stop_name);
		else return StopInfoMessage(stop_name, opt_stop_info.value());
	}
	else {
		return StopInfoErrorMessage(stop_name);
	}
}

} // end of anonymous namespace

std::string ShowInfo(const transport::catalogue::TransportCatalogue& tc, const  Query& q) {
	if (q.type == QueryType::BusInfo) {
		return (ShowBusInfo(tc, q.bus_name_info));
	}
	else if (q.type == transport::reader::QueryType::StopInfo) {
		return (ShowStopInfo(tc, q.stop_name_info));
	}
	else {
		return "error"s;
	}
}


} // end of namespace statistics

} // end of namespace reader

} // end of namespace transport

