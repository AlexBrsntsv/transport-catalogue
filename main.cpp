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

	transport::reader::input::Proccess(std::cin, ReadNumber(std::cin), transport_catalogue);	
	transport::reader::statistics::Proccess(std::cin, std::cout, ReadNumber(std::cin), transport_catalogue);
	
	return 0;
}