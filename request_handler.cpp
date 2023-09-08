#include "request_handler.h"
#include "json_reader.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */


// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "request_handler.h"
#include <math.h>


using transport::catalogue::BusInfo;
using namespace std::literals;

namespace transport {

namespace statistics {
		
void StatisticsTextOutput::Add(const BusInfo& bus_info, int) {
	double route_length = std::round(bus_info.route_length * 100) / 100;
	buffer_.push(
		"Bus "s + bus_info.name + ": "s +
		std::to_string(bus_info.stops_on_route) + " stops on route, "s +
		std::to_string(bus_info.unique_stops) + " unique stops, " +
		std::to_string(static_cast<int>(route_length)) + " route length, "s +
		std::to_string(bus_info.curvature) + " curvature"s
	);
}

void StatisticsTextOutput::Add(const std::string& stop_name, std::vector<std::string_view> buses, int) {
	std::string result;
	if (buses.empty()) {
		result = "Stop "s + stop_name + ": no buses"s;
	}
	else {
		result = "Stop "s + stop_name + ": buses"s;
		for (std::string_view bus : buses) {
			result += ' ';
			result += std::string(bus);
		}
	}
	buffer_.push (result);
}

void StatisticsTextOutput::AddError(std::string name, RequestError error_mark, int) {
	std::string result;
	if (error_mark == RequestError::Stop) {
		result =  "Stop "s + name + ": not found"s;
	}
	else if (error_mark == RequestError::Bus) {
		result = "Bus "s + name + ": not found"s;
	}
	buffer_.push(result);
}

void StatisticsTextOutput::Show() {
	while (!buffer_.empty()) {
		output_ << buffer_.front() << std::endl;
		buffer_.pop();
	}
}

bool StatisticsTextOutput::Ready() {
	return !buffer_.empty();
}


void StatisticsJsonOutput::Add(const transport::catalogue::BusInfo& bus_info, int id){	
	json::Dict node_to_add{
		{"curvature", bus_info.curvature},
		{"request_id" , id},
		{"route_length" , bus_info.route_length},
		{"stop_count", bus_info.stops_on_route},
		{"unique_stop_count", bus_info.unique_stops}
	};
	buffer_.emplace_back(json::Node(node_to_add));
}

void StatisticsJsonOutput::Add(const std::string&, std::vector<std::string_view> buses, int id) {
	json::Array buses_list;
	std::transform(
		buses.begin(),
		buses.end(),
		std::back_inserter(buses_list),
		[](const auto& element) { return json::Node(std::string(element)); }
	);

	json::Dict node_to_add{
		{"buses", buses_list},
		{"request_id" , id}
	};
	buffer_.emplace_back(json::Node(node_to_add));
}


void StatisticsJsonOutput::AddError(std::string, RequestError, int id){
	json::Dict node_to_add{
		{"request_id"s, id},
		{"error_message"s , "not found"s}
	};
	buffer_.emplace_back( json::Node(node_to_add) );
}

void StatisticsJsonOutput::Show() {
	output_ << json::Node(buffer_);
}
bool StatisticsJsonOutput::Ready() { return false; }


} // end of namespace statistics

} // end of namespace transport

