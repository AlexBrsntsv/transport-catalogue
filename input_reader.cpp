// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include <string_view>


static const std::string COMMAND_ADD_STOP = "Stop"s;
static const std::string COMMAND_BUS = "Bus"s;

// remove spaces before and after string
std::string_view trim_whitespace_surrounding(const std::string_view& s) {
	const char whitespace[]{ " \t\n" };
	const size_t first(s.find_first_not_of(whitespace));
	if (std::string::npos == first) { return {}; }
	const size_t last(s.find_last_not_of(whitespace));
	return s.substr(first, (last - first + 1));
}


//std::pair<std::string, std::string> Split(std::string line, char by) {
//	size_t pos = line.find(by);
//	std::string left = line.substr(0, pos);
//
//	if (pos < line.size() && pos + 1 < line.size()) {
//		return { left, line.substr(pos + 1) };
//	}
//	else {
//		return { left, std::string() };
//	}
//}

std::string_view GetStopName(std::string_view s) {
	const static std::string wrong_stop = "noname"s;
	s.remove_prefix(5); // remove comand "Stop "
	if (size_t colon_pos = s.find_first_of(':'); colon_pos != s.npos) {
		s.remove_prefix(s.size() - colon_pos);
		return (trim_whitespace_surrounding(s));
	}
	return wrong_stop;
}

Coordinates GetStopCoordinates(std::string_view s) {
	const char pref_char_to_remove[]{ " :" };
	std::string_view coordinates_str(s);
	coordinates_str.remove_prefix ( coordinates_str.find_first_not_of(pref_char_to_remove) );
	size_t comma_pos = coordinates_str.find_first_of(',');
	std::string lat_s (coordinates_str.substr(0, comma_pos));
	std::string lng_s(coordinates_str.substr(comma_pos + 2) );
	return { std::stod(lat_s, 0), std::stod(lng_s, 0) };
}

QueryType GetQueryType(std::string_view s) {
	std::string_view cmd(s);
	if (size_t end_pos = cmd.find_first_of(' '); end_pos == cmd.npos) {
		return QueryType::Unknown;
	}
	else{
		cmd.remove_suffix(cmd.size() - end_pos);
	}
	if (cmd == COMMAND_ADD_STOP) {
		return QueryType::AddStop;
	}
	else if (cmd == COMMAND_BUS) {
		if (size_t colon_pos = s.find_first_of(':'); colon_pos == s.npos) {
			return QueryType::BusInfo;
		}
		else {
			return QueryType::AddBus;
		}
	}
	else {
		return QueryType::Unknown;
	}

}


//std::istream& operator>> (std::istream& in, Query& q) {	
//	q.type = QueryType::Unknown; // initial value
//	std::string s;
//	
//	getline(in,s);
//	// remove space from begin
//	if (size_t start_pos = s.find_first_not_of(' '); start_pos != s.npos) s = s.substr(start_pos);
//	else return in;
//	
//	q.type = GetQueryType(s);
//	switch (q.type) {
//	case QueryType::AddBus:
//		break;
//	case QueryType::AddStop:
//		q.stop.name = std::string( GetStopName(s) );
//		break;
//	case QueryType::BusInfo:	
//		break;
//	case QueryType::Unknown:
//		break;
//	}
//
//
//	return in;
//
//
//}


std::istream& operator>> (std::istream& in, Query& q) {
	q.type = QueryType::Unknown; // initial value
	std::string input_str;
	getline(in, input_str);
	std::string_view s(input_str);
	// remove space from begin
	if (size_t start_pos = s.find_first_not_of(' '); start_pos != s.npos) s.remove_prefix(start_pos);
	else return in;

	q.type = GetQueryType(s);
	switch (q.type) {
	case QueryType::AddBus:
		break;
	case QueryType::AddStop:
		s.remove_prefix( COMMAND_ADD_STOP.size() );		
		q.stop.name = std::string(GetStopName(s));
		s.remove_prefix( q.stop.name.size() );
		q.stop.coordinates = GetStopCoordinates(s);
		break;
	case QueryType::BusInfo:
		break;
	case QueryType::Unknown:
		break;
	}

	return in;


}
