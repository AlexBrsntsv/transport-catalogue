#include <iostream>
#include "tests.h"
#include <sstream>
#include <iomanip>
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"
using namespace std;
using namespace transport;

int ReadNumber(std::istream& is) {		
	std::string s;
	std::getline(is, s);
	return std::stoi(s);
}

int main() {	
	//transport::tests::Catalogue();
	//transport::tests::Input();
	
	
	transport::catalogue::TransportCatalogue transport_catalogue;
	transport::input::query_queue::InputQueryQueue input_queue;
	transport::input::InputReader input_reader;
		
	for (size_t i = ReadNumber(std::cin); i > 0; --i) {
		input_queue.AddQuery( input_reader.GetQuery(std::cin) );
	}
	
	while (!input_queue.Stops().empty()) {
		ProccessAddStopQuery(transport_catalogue, input_queue.Stops().front() );
		input_queue.Stops().pop();
	}

	while (!input_queue.Lengths().empty()) {
		ProccessAddStopsLengthsQuery(transport_catalogue, input_queue.Lengths().front());
		input_queue.Lengths().pop();
	}

	while (!input_queue.Busies().empty()) {
		ProccessAddBusQuery(transport_catalogue, input_queue.Busies().front());
		input_queue.Busies().pop();
	}
	

	for (size_t i = ReadNumber(std::cin); i > 0; --i) {
		input_queue.AddQuery(input_reader.GetQuery(std::cin));
	}

	while (!input_queue.Info().empty()) {
		std::cout << transport::output::ShowInfo(transport_catalogue, input_queue.Info().front()) << std::endl;
		input_queue.Info().pop();
	}
	
	return 0;
}