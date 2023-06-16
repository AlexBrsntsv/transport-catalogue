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
	std::pair <std::string, std::vector<std::string>> bus_info;
	Stop stop;
};



std::istream& operator>> (std::istream& in, Query& q);

class InputQueryQueue {
public:
	InputQueryQueue() = default;
	void AddQuery(Query&& q);	
	std::queue<Query>& Busies();
	std::queue<Query>& Stops();

private:
	std::queue<Query> AddBusQueryQueue;
	std::queue<Query> AddStopQueryQueue;
};

void ProccessAddStopQuery(TransportCatalogue& trans_ctlg, Query& q); 
void ProccessAddBusQuery(TransportCatalogue& trans_ctlg, Query& q);


// internal using
std::pair<std::string, bool> ParseBusName(std::istream& in);
std::vector<std::string> ParseBusRoute(std::istream& in);