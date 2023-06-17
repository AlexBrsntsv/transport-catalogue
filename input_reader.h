#pragma once
#include "transport_catalogue.h"
#include <sstream>
#include <queue>

using namespace std::literals;


enum class QueryType {
	AddBus,
	AddStop,
	BusInfo,
	BusOperation,
	Unknown
};

struct Query {
	QueryType type;
	std::string bus_name_info;
	BusNew bus_new;
	Stop stop;
};



class InputReader {
public:
	Query GetQuery(std::istream& in);

private:
	inline static const std::string COMMAND_ADD_STOP = "Stop"s;
	inline static const std::string COMMAND_BUS = "Bus"s;

	static std::pair<std::string, bool> ParseBusName(std::istream& in);
	static std::vector<std::string> ParseBusRoute(std::istream& in);
	static Query ParseAddStopCommand(std::istream& in);
	static std::string TrimWhitespaceSurrounding(const std::string& s);
	static std::pair<std::string, std::string> Split(std::string line, char by);
	static QueryType ToQueryType(const std::string& s);
};


class InputQueryQueue {
public:
	InputQueryQueue() = default;
	void AddQuery(const Query& q);	
	std::queue<Query>& Busies();
	std::queue<Query>& Stops();
private:
	std::queue<Query> AddStopQueryQueue;
	std::queue<Query> AddBusQueryQueue;
};


void ProccessAddStopQuery(TransportCatalogue& transport_catalogue, Query& q);
void ProccessAddBusQuery(TransportCatalogue& transport_catalogue, Query& q);