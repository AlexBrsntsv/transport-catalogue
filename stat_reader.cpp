// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "stat_reader.h"
#include <math.h>


using transport::catalogue::BusInfo;
using namespace std::literals;

namespace transport {

namespace statistics {
		
void StatisticsTextOutput::Add(const BusInfo& bus_info) {
	double route_length = std::round(bus_info.route_length * 100) / 100;
	buffer_.push(
		"Bus "s + bus_info.name + ": "s +
		std::to_string(bus_info.stops_on_route) + " stops on route, "s +
		std::to_string(bus_info.unique_stops) + " unique stops, " +
		std::to_string(static_cast<int>(route_length)) + " route length, "s +
		std::to_string(bus_info.curvature) + " curvature"s
	);
}

void StatisticsTextOutput::Add(const std::string& stop_name, std::vector<std::string_view> buses) {
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

void StatisticsTextOutput::AddError(std::string name, RequestError error_mark) {
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


} // end of namespace statistics

} // end of namespace transport

