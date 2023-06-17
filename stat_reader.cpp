// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "stat_reader.h"



std::string DisplayBusInfo(const TransportCatalogue& trans_ctlg, const std::string& bus_name){
	Bus bus = trans_ctlg.FindBus(bus_name);
	//if (!BusIsValid(bus)) return "Bus " + bus_name + ": not found.";

	return std::string{};
}

