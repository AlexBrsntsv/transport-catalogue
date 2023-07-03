// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include <string_view>
#include <assert.h>
#include "stat_reader.h"

namespace { // file scope functions

using transport::reader::Query;
using transport::reader::QueryType;

inline static const std::string COMMANDS_STOP = "Stop"s;
inline static const std::string COMMANDS_BUS = "Bus"s;


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

geo::Coordinates ParseStopCoordinates(std::string_view s) {
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

Query GetQuery(std::istream& is) {
	std::string s;
	std::getline(is, s);
	return ParseQuery(s);
}

} // end of anonymous namespace

namespace transport{

namespace reader {

size_t GetPriority(QueryType q) {
	size_t result = 0;
	switch (q) {
	case QueryType::AddStop:
		result = 0; // highest priority
		break;

	case QueryType::Distancies:
		result = 1;
		break;
	case QueryType::AddBus:
		result = 2;
		break;

	case QueryType::BusInfo: // have same priority
	case QueryType::StopInfo:
		result = 3;
		break;
	default:
		result = static_cast<size_t>(QueryType::QueryTypesAmount);
		break;
	}
	return result;
}

Query TextReader::ExtractQuery(std::istream& input_) {
	return GetQuery(input_);
} // extracts query from source stream

bool operator==(const Query& lhs, const Query& rhs) {
	return
		lhs.bus_name_info == rhs.bus_name_info &&
		lhs.busname_to_route.first == rhs.busname_to_route.first &&
		lhs.busname_to_route.second == rhs.busname_to_route.second &&
		lhs.stop == rhs.stop &&
		lhs.type == rhs.type;
}

namespace { // file scope function

void QueryTypeHandler(Query&& q, transport::catalogue::TransportCatalogue& db, std::ostream& out) {
	switch (q.type) {
	case QueryType::AddStop:
		db.AddStop(q.stop);
		break;
	case QueryType::AddBus:
		db.AddBus(q.busname_to_route.first, q.busname_to_route.second);
		break;
	case QueryType::Distancies:
		for (const auto& [to_stop, distance] : q.stop_distancies) {
			db.SetStopsLength(q.stop.name, to_stop, distance);
		}
		break;
	case QueryType::BusInfo:
	case QueryType::StopInfo:
		out << reader::statistics::ShowInfo(db, q) << std::endl;
		break;
	}
}

} // end of 2 anonymous namespace 


void Process(
	InputReader* reader, 
	int query_num, 
	transport::catalogue::TransportCatalogue& db, 
	std::ostream& out) 
{
	reader->Process(query_num);
	while (!reader->Empty()) {
		QueryTypeHandler(reader->GetNext(), db, out);
	}
}

} // end of namespace reader

} // end of namespace transport
