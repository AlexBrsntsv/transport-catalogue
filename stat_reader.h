#pragma once

#include <string>
#include "transport_catalogue.h"
#include "input_reader.h"



//std::string ShowBusInfo(const TransportCatalogue& transport_catalogue, const std::string& bus_name);
//std::string ShowStopInfo(const TransportCatalogue& transport_catalogue, const std::string& stop_name);
std::string ShowInfo(const TransportCatalogue& tc, const Query& q);