#pragma once
#include "transport_catalogue.h"
#include <sstream>
#include <queue>

using namespace std::literals;


enum class QueryType {
	Invalid,
	AddBus,
	AddStop,
	BusInfo,		
};

struct Query {
	QueryType type;
	std::string bus_name_info;
	BusNew bus_new;
	Stop stop;
};



class InputReader {
public:
	Query GetQuery(std::istream& is);

private:
	inline static const std::string COMMAND_ADD_STOP = "Stop"s;
	inline static const std::string COMMAND_BUS = "Bus"s;

	static std::string_view TrimWhitespaceSurrounding(std::string_view s);
	static std::optional<std::pair<std::string_view, std::string_view>> Split(std::string_view line, char by);
	static std::vector<std::string> ParseRoute(std::string_view s);
	static Coordinates ParseStopCoordinates(std::string_view s);
	static Query ParseQuery(std::string_view s);
};


class InputQueryQueue {
public:
	InputQueryQueue() = default;
	void AddQuery(const Query& q);	
	std::queue<Query>& Busies();
	std::queue<Query>& Stops();
	std::queue<Query>& Info();
private:
	std::queue<Query> AddStopQueryQueue;
	std::queue<Query> AddBusQueryQueue;
	std::queue<Query> GetBusInfoQueryQueue;
};

void ProccessAddStopQuery(TransportCatalogue& transport_catalogue, const Query& q);
void ProccessAddBusQuery(TransportCatalogue& transport_catalogue, const Query& q);


