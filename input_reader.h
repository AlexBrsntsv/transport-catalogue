#pragma once
#include "transport_catalogue.h"
#include <sstream>
#include <queue>


using namespace std::literals;

namespace transport {

namespace reader {

enum class QueryType {
	Invalid,
	AddBus,
	AddStop,
	BusInfo,
	StopInfo
};

struct Query {
	QueryType type;
	std::string stop_name_info;
	std::string bus_name_info;
	std::pair <std::string, std::vector<std::string>> busname_to_route;
	transport::catalogue::Stop stop;
	std::vector<std::pair<std::string, int>> stop_distancies;
};

bool operator==(const Query& lhs, const Query& rhs);


Query GetQuery(std::istream& is);

namespace detail {

inline static const std::string COMMANDS_STOP = "Stop"s;
inline static const std::string COMMANDS_BUS = "Bus"s;

std::string_view TrimWhitespaceSurrounding(std::string_view s);
std::optional<std::pair<std::string_view, std::string_view>> Split(std::string_view line, char by);
std::vector<std::string> ParseRoute(std::string_view s);
Coordinates ParseStopCoordinates(std::string_view s);
Query ParseQuery(std::string_view s);
std::pair<std::string, int> ParseDistance(std::string_view s);
std::vector<std::pair<std::string, int>> ParseStopDistances(std::string_view s);

class InputQueryQueue {
public:
	InputQueryQueue() = default;
	void AddQuery(const Query& q);
	std::queue<Query>& Busies();
	std::queue<Query>& Stops();
	std::queue<Query>& Lengths();
private:
	std::queue<Query> AddStopsLengthsQueryQueue;
	std::queue<Query> AddStopQueryQueue;
	std::queue<Query> AddBusQueryQueue;

};

} // end of namespace detail

namespace input {

	void Proccess(std::istream& is, int query_num, transport::catalogue::TransportCatalogue& transport_catalogue);

}


} // end of namespace reader


} // end of namespace transport