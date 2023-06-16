// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include <string_view>
#include <execution>



static const std::string COMMAND_ADD_STOP = "Stop"s;
static const std::string COMMAND_BUS = "Bus"s;

// remove spaces before and after string
std::string trim_whitespace_surrounding(const std::string& s) {
	const char whitespace[]{ " \t\n" };
	const size_t first(s.find_first_not_of(whitespace));
	if (std::string::npos == first) { return {}; }
	const size_t last(s.find_last_not_of(whitespace));
	return s.substr(first, (last - first + 1));
}


std::pair<std::string, std::string> Split(std::string line, char by) {
	if (line.empty()) return  { std::string(), std::string() };
	size_t pos = line.find(by);
	std::string left = line.substr(0, pos);

	if (pos < line.size() && pos + 1 < line.size()) {
		return { left, line.substr(pos + 1) };
	}
	else {
		return { left, std::string() };
	}
}

QueryType ToQueryType(std::string s) {
	if (s == "Stop"s) return QueryType::AddStop;
	else if (s == "Bus"s) return QueryType::BusOperation;
	else return QueryType::Unknown;
}


std::pair<std::string, bool> ParseBusName(std::istream& in) {
	bool bus_info_mark = false;
	char c;
	std::string name;
	for ( ; in.get(c); ) {
		if (c == '\n' || in.eof()) {
			bus_info_mark = true;
			break;
		}
		else if (c == ':') {
			bus_info_mark = false;
			break;
		}
		else {
			name += c;
		}
	}
	return { trim_whitespace_surrounding(name), bus_info_mark };
}

std::vector<std::string> ParseBusRoute(std::istream& in) {
	std::vector<std::string> result;
	std::string route;
	getline(in, route);
	char split_symbol;
	if (route.find_first_of('>') != route.npos) {
		split_symbol = '>';
	}
	else {
		split_symbol = '-';
	}

	for (auto string_parts = Split(route, split_symbol); !string_parts.first.empty(); string_parts = Split(string_parts.second, split_symbol)) {
		result.push_back( trim_whitespace_surrounding(string_parts.first) );
	}

	if (split_symbol == '-') {
		std::vector<std::string> v( std::next(result.rbegin()), result.rend() );
		std::copy(v.begin(), v.end(), std::back_inserter(result));
	}
	return result;
}


Query ParseAddStopCommand(std::istream& in) {
	Query q;
	q.type = QueryType::AddStop;

	bool first_name_part = true;
	do {
		std::string name_part;
		in >> name_part;

		if (first_name_part) {
			first_name_part = false;
		}
		else {
			q.stop.name += ' ';
		}
		if (name_part == ":"s) break;
		q.stop.name += name_part;
	} while (q.stop.name.back() != ':');
	q.stop.name.pop_back();
	
	char comma;
	in >> q.stop.coordinates.lat >> comma >> q.stop.coordinates.lng;
	return q;
}

std::istream& operator>> (std::istream& in, Query& q) {
	std::string cmd;
	in >> cmd;
	q.type = ToQueryType(cmd);
	switch (q.type) {
	case QueryType::AddStop:
		q = ParseAddStopCommand(in);
		break;
	case QueryType::BusOperation: {
		const auto [bus_name, bus_info_cmd_mark] = ParseBusName(in);
		if (bus_info_cmd_mark) {
			q.type = QueryType::BusInfo;
			q.bus_info.first = bus_name;
			return in;
		}
		else {
			q.type = QueryType::AddBus;
			q.bus_info.first = bus_name;
			q.bus_info.second = ParseBusRoute(in);
		}
		break;
	}

	case QueryType::Unknown:
		break;
	}
	return in;
}

void ProccessAddStopQuery(TransportCatalogue& trans_ctlg, Query& q) {
	trans_ctlg.AddStop(q.stop);
}

void ProccessAddBusQuery(TransportCatalogue& trans_ctlg, Query& q) {
	trans_ctlg.AddBus( q.bus_info.first, q.bus_info.second );
}



void InputQueryQueue::AddQuery(Query&& q) {
	switch (q.type) {
	case QueryType::AddBus:
		AddBusQueryQueue.push(q);
		break;

	case QueryType::AddStop:
		AddStopQueryQueue.push(q);
		break;
	}
}
	
std::queue<Query>& InputQueryQueue::Busies() { return AddBusQueryQueue; }

std::queue<Query>& InputQueryQueue::Stops() { return AddStopQueryQueue; }






