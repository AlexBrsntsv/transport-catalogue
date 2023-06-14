#include <iostream>
#include "tests.h"
#include <sstream>
#include <iomanip>
using namespace std;




int main() {
	//TransportCatalogueRunTests();
	//InputReaderRunTests();	
	std::stringstream ss ("Bus 256\n"s 
		"Bus 257\n"s
	
	);
	string cmd;
	ss >> cmd;
	string name;
	bool first_name_part = true;
	bool bus_info_reques = false;

	do {
		string name_part;
		if (!(ss >> name_part)) {			;
			bus_info_reques = true;
			break;
		}

		if (first_name_part) {
			first_name_part = false;
		}
		else {
			name += ' ';
		}
		if (name_part == ":"s) break;
		name += name_part;
	} while (name.back() != ':');
	cout << name << " "s << boolalpha << bus_info_reques << endl;
	

	//double x, y;
	//char comma;
	//ss >> x >> comma >> y;

	//cout << setprecision(8);
	//cout << "command: "s << cmd << endl;
	//cout << "name: "s << name << "<" << endl;
	//cout << "("s << x << ", "s << y << ")"s << endl;




	return 0;
}