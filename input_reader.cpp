// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include <string_view>
#include <assert.h>


// remove spaces before and after string
std::string TrimWhitespaceSurrounding(const std::string& s) {
	const char whitespace[]{ " \t\n" };
	const size_t first(s.find_first_not_of(whitespace));
	if (std::string::npos == first) { return {}; }
	const size_t last(s.find_last_not_of(whitespace));
	return s.substr(first, (last - first + 1));
}


// remove spaces before and after string
std::string_view InputReader::TrimWhitespaceSurrounding(std::string_view s) {
	const char whitespace[]{ " \t\n" };
	const size_t first(s.find_first_not_of(whitespace));
	if (std::string::npos == first) { return {}; }
	const size_t last(s.find_last_not_of(whitespace));
	return s.substr(first, (last - first + 1));
}

std::optional<std::pair<std::string_view, std::string_view>> InputReader::Split(std::string_view line, char by) {
	if (line.empty()) return std::nullopt;
	size_t pos = line.find(by);	
	std::string_view left = line.substr(0, pos);

	if (pos < line.size() && pos + 1 < line.size()) {
		return std::optional(std::pair{ left, line.substr(pos + 1) });
	}
	else {
		return std::optional(std::pair{ left, std::string() });
	}
}

std::pair<std::string, int> InputReader::ParseDistance(std::string_view s) {
	if ( auto opt = Split(s, 'm'); opt.has_value() ) {
		int distance = std::stoi(std::string(TrimWhitespaceSurrounding(opt->first)));
		opt->second.remove_prefix( opt->second.find_first_of('t') + "to "s.size() );
		std::string stop_name ( TrimWhitespaceSurrounding(opt->second) );
		return { stop_name , distance };
	}
	return {};
}


std::vector<std::pair<std::string, int>> InputReader::ParseStopDistances(std::string_view s) {
	std::vector<std::pair<std::string, int>> result;
	for (auto opt = Split(s, ','); opt.has_value(); opt = Split(opt->second, ',')) {
		result.push_back(ParseDistance(opt->first));
	}
	return result;
}



std::vector<std::string> InputReader::ParseRoute(std::string_view s) {
	std::vector<std::string> route;
	char split_symbol;
	if (size_t devider_pos = s.find_first_of(">-"); devider_pos != s.npos) {
		split_symbol = s[devider_pos];
	}
	else {
		return route;
	}	
		
	for (auto opt = Split(s, split_symbol); opt.has_value(); opt = Split(opt->second, split_symbol)) {
		route.push_back(std::string(TrimWhitespaceSurrounding(opt->first)));
	}		
	
	if (split_symbol == '-') {
		route.resize( 2 * route.size() - 1);		
		std::copy(route.begin(), std::prev(route.end()), route.rbegin() );
	}
	return route;
}

Coordinates InputReader::ParseStopCoordinates(std::string_view s) {
	auto opt_coordinates = Split(s, ',');
	double lat = std::stod( std::string( TrimWhitespaceSurrounding(opt_coordinates->first) ) );
	opt_coordinates = Split(opt_coordinates->second, ',');
	double lng = std::stod( std::string( TrimWhitespaceSurrounding(opt_coordinates->first) ) );
	return { lat,lng };
}

Query InputReader::ParseQuery(std::string_view s) {

	s = TrimWhitespaceSurrounding(s); // remove spaces
	Query q{};

	if (auto opt = Split(s, ':'); opt.has_value() && opt->first != s) {
		if (opt->first.substr(0, COMMANDS_BUS.size()) == COMMANDS_BUS) {
			q.type = QueryType::AddBus;
			opt->first.remove_prefix(COMMANDS_BUS.size());
			q.bus_new.name = std::string( TrimWhitespaceSurrounding(opt->first) );
			auto route = ParseRoute(opt->second);
			q.bus_new.route.swap(route);
			return q;
		}
		else if (opt->first.substr( 0, COMMANDS_STOP.size() ) == COMMANDS_STOP) {
			opt->first.remove_prefix( COMMANDS_STOP.size() );
			q.type = QueryType::AddStop;
			q.stop.name = std::string( TrimWhitespaceSurrounding(opt->first) );
			q.stop.coordinates = ParseStopCoordinates(opt->second);
			opt = Split(opt->second, ','); // remove coordinate part
			if (opt = Split(opt->second, ',');	!opt->second.empty()) {
				q.stop_distancies = ParseStopDistances(opt->second);
			}			
			return q;
		}
		else {
			q.type = QueryType::Invalid;
			return q;
		}
	}
	else {
		if (s.substr(0, COMMANDS_BUS.size()) == COMMANDS_BUS) {
			q.type = QueryType::BusInfo;
			s.remove_prefix(COMMANDS_BUS.size());
			q.bus_name_info = std::string( TrimWhitespaceSurrounding(s) );
			return q;
		}
		else if( s.substr(0, COMMANDS_STOP.size()) == COMMANDS_STOP){
			q.type = QueryType::StopInfo;
			s.remove_prefix(COMMANDS_STOP.size());
			q.stop_name_info = std::string(TrimWhitespaceSurrounding(s));
			return q;
		}
		else {
			q.type = QueryType::Invalid;
			return q;
		}
	}
}

Query InputReader::GetQuery(std::istream& is) {
	std::string s;
	std::getline(is, s);
	return ParseQuery(s);
}

void ProccessAddStopQuery(TransportCatalogue& transport_catalogue, const Query& q) {
	transport_catalogue.AddStop(q.stop);
}

void ProccessAddBusQuery(TransportCatalogue& transport_catalogue, const Query& q) {
	transport_catalogue.AddBus( q.bus_new );
}

void InputQueryQueue::AddQuery(const Query& q) {
	switch (q.type) {
	case QueryType::AddBus:
		AddBusQueryQueue.push(q);
		break;

	case QueryType::AddStop:
		AddStopQueryQueue.push(q);
		break;

	case QueryType::StopInfo:
	case QueryType::BusInfo:
		GetInfoQueryQueue.push(q);
		break;

	case QueryType::Invalid:		
		break;
	}
}
	
std::queue<Query>& InputQueryQueue::Busies() { return AddBusQueryQueue; }
std::queue<Query>& InputQueryQueue::Stops() { return AddStopQueryQueue; }
std::queue<Query>& InputQueryQueue::Info() { return GetInfoQueryQueue; }
