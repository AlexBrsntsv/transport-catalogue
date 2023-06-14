#pragma once
#include "transport_catalogue.h"
#include <sstream>

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
	std::pair <std::string, std::vector<std::string>> add_bus_info;
	Stop stop;
};

QueryType GetQueryType(std::string_view s);

std::istream& operator>> (std::istream& in, Query& q);