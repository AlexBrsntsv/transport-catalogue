#include <iostream>
#include "tests.h"
#include <sstream>
#include <iomanip>
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"
using namespace std;




int main() {
	
	InputReaderRunTests();
	std::cout << "OK"s << std::endl;
	/*
	TransportCatalogueRunTests();
	InputReaderRunTests();	
	TransportCatalogue transport_catalogue;
	InputQueryQueue input_queue;
	static InputReader input_reader;

	size_t cmd_num = 0;
	std::cin >> cmd_num;	
		
	for (size_t i = 0; i < cmd_num; ++i) {
		input_queue.AddQuery( input_reader.GetQuery(std::cin) );
	}
	
	while (!input_queue.Stops().empty()) {
		ProccessAddStopQuery(transport_catalogue, input_queue.Stops().front() );
		input_queue.Stops().pop();
	}

	while (!input_queue.Busies().empty()) {
		ProccessAddBusQuery(transport_catalogue, input_queue.Busies().front());
		input_queue.Busies().pop();
	}

	std::cin >> cmd_num;
	for (size_t i = 0; i < cmd_num; ++i) {
		Query q = input_reader.GetQuery(std::cin);
		std::cout << ShowBusInfo(transport_catalogue, q.bus_name_info);		
	}
	*/
	return 0;
}