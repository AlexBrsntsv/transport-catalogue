// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include <string_view>
#include <assert.h>


namespace transport {

namespace reader {

bool operator==(const Query& lhs, const Query& rhs) {
	return
		lhs.bus_name_info == rhs.bus_name_info &&
		lhs.busname_to_route.first == rhs.busname_to_route.first &&
		lhs.busname_to_route.second == rhs.busname_to_route.second &&
		lhs.stop == rhs.stop &&
		lhs.type == rhs.type;
}

namespace detail{


// remove spaces before and after string
std::string TrimWhitespaceSurrounding(const std::string& s) {
	const char whitespace[]{ " \t\n" };
	const size_t first(s.find_first_not_of(whitespace));
	if (std::string::npos == first) { return {}; }
	const size_t last(s.find_last_not_of(whitespace));
	return s.substr(first, (last - first + 1));
}

// remove spaces before and after string
std::string_view TrimWhitespaceSurrounding(std::string_view s) {
	const char whitespace[]{ " \t\n" };
	const size_t first(s.find_first_not_of(whitespace));
	if (std::string::npos == first) { return {}; }
	const size_t last(s.find_last_not_of(whitespace));
	return s.substr(first, (last - first + 1));
}

std::optional<std::pair<std::string_view, std::string_view>> Split(std::string_view line, char by) {
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

std::pair<std::string, int> ParseDistance(std::string_view s) {
	if (auto opt = Split(s, 'm'); opt.has_value()) {
		int distance = std::stoi(std::string(TrimWhitespaceSurrounding(opt->first)));
		opt->second.remove_prefix(opt->second.find_first_of('t') + "to "s.size());
		std::string stop_name(TrimWhitespaceSurrounding(opt->second));
		return { stop_name , distance };
	}
	return {};
}


std::vector<std::pair<std::string, int>> ParseStopDistances(std::string_view s) {
	std::vector<std::pair<std::string, int>> result;
	for (auto opt = Split(s, ','); opt.has_value(); opt = Split(opt->second, ',')) {
		result.push_back(ParseDistance(opt->first));
	}
	return result;
}



std::vector<std::string> ParseRoute(std::string_view s) {
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
		route.resize(2 * route.size() - 1);
		std::copy(route.begin(), std::prev(route.end()), route.rbegin());
	}
	return route;
}

Coordinates ParseStopCoordinates(std::string_view s) {
	auto opt_coordinates = Split(s, ',');
	double lat = std::stod(std::string(TrimWhitespaceSurrounding(opt_coordinates->first)));
	opt_coordinates = Split(opt_coordinates->second, ',');
	double lng = std::stod(std::string(TrimWhitespaceSurrounding(opt_coordinates->first)));
	return { lat,lng };
}

Query ParseQuery(std::string_view s) {

	s = TrimWhitespaceSurrounding(s); // remove spaces
	Query q{};

	if (auto opt = Split(s, ':'); opt.has_value() && opt->first != s) {
		if (opt->first.substr(0, COMMANDS_BUS.size()) == COMMANDS_BUS) {
			q.type = QueryType::AddBus;
			opt->first.remove_prefix(COMMANDS_BUS.size());
			q.busname_to_route.first = std::string(TrimWhitespaceSurrounding(opt->first));
			auto route = ParseRoute(opt->second);
			q.busname_to_route.second.swap(route);
			return q;
		}
		else if (opt->first.substr(0, COMMANDS_STOP.size()) == COMMANDS_STOP) {
			opt->first.remove_prefix(COMMANDS_STOP.size());
			q.type = QueryType::AddStop;
			q.stop.name = std::string(TrimWhitespaceSurrounding(opt->first));
			q.stop.coordinates = ParseStopCoordinates(opt->second);
			// remove coordinate part and check if there is distance part
			opt = Split(opt->second, ',');
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
			q.bus_name_info = std::string(TrimWhitespaceSurrounding(s));
			return q;
		}
		else if (s.substr(0, COMMANDS_STOP.size()) == COMMANDS_STOP) {
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





void InputQueryQueue::AddQuery(const Query& q) {
	switch (q.type) {
	case QueryType::AddBus:
		AddBusQueryQueue.push(q);
		break;

	case QueryType::AddStop:
		if (q.stop_distancies.empty()) {
			AddStopQueryQueue.push(q);
		}
		else {
			AddStopQueryQueue.push({ QueryType::AddStop, {}, {}, {}, q.stop, {} });
			AddStopsLengthsQueryQueue.push(q);
		}
		break;
	case QueryType::BusInfo:
	case QueryType::StopInfo:
	case QueryType::Invalid:
		throw std::invalid_argument("Invalid command"s);
		break;
	}
}

std::queue<Query>& InputQueryQueue::Busies() { return AddBusQueryQueue; }
std::queue<Query>& InputQueryQueue::Stops() { return AddStopQueryQueue; }
std::queue<Query>& InputQueryQueue::Lengths() { return AddStopsLengthsQueryQueue; }

} // end of namespace detail

namespace input{

void Proccess(std::istream& is, int query_num,  transport::catalogue::TransportCatalogue& transport_catalogue) {
	detail::InputQueryQueue input_queue;
	for (int i = 0; i < query_num; ++i) {
		input_queue.AddQuery(reader::GetQuery(is));
	}

	while (!input_queue.Stops().empty()) {
		auto& next_queue = input_queue.Stops().front();
		transport_catalogue.AddStop(next_queue.stop);
		input_queue.Stops().pop();
	}

	while (!input_queue.Lengths().empty()) {
		auto& next_queue = input_queue.Lengths().front();
		for (const auto& [to_stop, distance] : next_queue.stop_distancies) {
			transport_catalogue.AddStopsLength(next_queue.stop.name, to_stop, distance);
		}
		input_queue.Lengths().pop();
	}

	while (!input_queue.Busies().empty()) {
		auto& next_queue = input_queue.Busies().front();
		transport_catalogue.AddBus(next_queue.busname_to_route.first, next_queue.busname_to_route.second);
		input_queue.Busies().pop();
	}
}

} // end of namespace input



Query GetQuery(std::istream& is) {
	std::string s;
	std::getline(is, s);
	return detail::ParseQuery(s);
}

} // end of namespace reader

} // end of namespace transport