#include <iostream>
#include "tests.h"
#include <sstream>
#include <iomanip>
#include "input_reader.h"
#include "transport_catalogue.h"
using namespace std;




int main() {
	//TransportCatalogueRunTests();
	//InputReaderRunTests();	
	TransportCatalogue transport_catalogue;
	InputQueryQueue input_queue;
	size_t cmd_num = 0;
	std::cin >> cmd_num;
	for (size_t i = 0; i < cmd_num; ++i) {
		Query q;
		cin >> q;
		input_queue.AddQuery(std::move(q));
	}
	while (!input_queue.Stops().empty()) {
		ProccessAddStopQuery(transport_catalogue, input_queue.Stops().front() );
		input_queue.Stops().pop();
	}

	while (!input_queue.Busies().empty()) {
		ProccessAddBusQuery(transport_catalogue, input_queue.Busies().front());
		input_queue.Busies().pop();
	}



	return 0;
}