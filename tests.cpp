
#include "tests.h"
#include "transport_catalogue.h"
#include "input_reader.h"
#include <cassert>
#include <algorithm>
#include <sstream>
#include <iostream>


using namespace std::literals;




//===============================================================
// input_reader tests

//void ParseRouteTest() {
//	{
//		std::string route = " Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye "s;
//		std::vector<std::string> ethalon = {
//			"Biryulyovo Zapadnoye"s,
//			"Biryusinka"s,
//			"Universam"s,
//			"Biryulyovo Tovarnaya"s,
//			"Biryulyovo Passazhirskaya"s,
//			"Biryulyovo Zapadnoye"s
//		};
//		auto result = ParseRoute(route);
//		assert(ethalon == result);
//	}
//
//	{
//		std::string route = " Tolstopaltsevo - Marushkino - Rasskazovka "s;
//		std::vector<std::string> ethalon = {
//			"Tolstopaltsevo"s,
//			"Marushkino"s,
//			"Rasskazovka"s,
//			"Marushkino"s,
//			"Tolstopaltsevo"s
//		};
//		auto result = ParseRoute(route);
//		assert(ethalon == result);
//	}
//}

//void ParseStopCoordinatesTest() {
//	{
//		Coordinates ethalon{ 55.581065,  37.648390 };
//		assert(ethalon == ParseStopCoordinates(" 55.581065,  37.648390 "s));
//	}
//}

bool operator==(const Query& lhs, const Query& rhs) {
	return
		lhs.bus_name_info == rhs.bus_name_info &&
		lhs.bus_new.name == rhs.bus_new.name &&
		lhs.bus_new.route == rhs.bus_new.route &&
		lhs.stop == rhs.stop &&
		lhs.type == rhs.type;
}
// old style
//void InputReaderInputOperatorTest() {
//	
//	std::istringstream iss(
//		"Stop Tolstopaltsevo : 55.611087, 37.208290\n"
//		"Stop Marushkino : 55.595884, 37.209755\n"s
//		"Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"s
//		"Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"s
//		"Stop Rasskazovka : 55.632761, 37.333324\n"s
//		"Stop   Biryulyovo Zapadnoye : 55.574371, 37.651700\n"s
//		"Stop Biryusinka : 55.581065, 37.648390\n"s
//		"Stop Universam : 55.587655, 37.645687\n"s
//		"Stop Biryulyovo Tovarnaya : 55.592028, 37.653656\n"s
//		"Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164\n"s
//		"Bus 256\n"s
//		"Bus 750\n"s
//		"Bus    751\n"s
//		"SBus    751\n"s
//		"Stap Biryulyovo Passazhirskaya : 55.580999, 37.659164\n"s
//		"Stop Biryulyovo Passazhirskaya\n"s
//		"Stop Universam"s
//	);
//
//	
//		
//	std::vector<Query> queries_ethalon = {
//		{ QueryType::AddStop, {}, {}, {}, {"Tolstopaltsevo"s,55.611087, 37.208290}},
//		{ QueryType::AddStop, {}, {}, {}, {"Marushkino"s,55.595884, 37.209755} },
//		{ QueryType::AddBus, {}, {}, BusNew{"256"s, {"Biryulyovo Zapadnoye"s, "Biryusinka"s, "Universam"s, "Biryulyovo Tovarnaya"s, "Biryulyovo Passazhirskaya"s, "Biryulyovo Zapadnoye"s} },{} },
//		{ QueryType::AddBus, {}, {}, BusNew{"750"s, {"Tolstopaltsevo"s, "Marushkino"s, "Rasskazovka"s, "Marushkino"s, "Tolstopaltsevo"s} }, {} },
//		{ QueryType::AddStop, {}, {}, {}, {"Rasskazovka"s,55.632761, 37.333324} },
//		{ QueryType::AddStop, {}, {}, {}, {"Biryulyovo Zapadnoye"s,55.574371, 37.651700} },
//		{ QueryType::AddStop, {}, {}, {}, {"Biryusinka"s,55.581065, 37.648390} },
//		{ QueryType::AddStop, {}, {}, {}, {"Universam"s,55.587655, 37.645687} },
//		{ QueryType::AddStop, {}, {}, {}, {"Biryulyovo Tovarnaya"s,55.592028, 37.653656} },
//		{ QueryType::AddStop, {}, {}, {}, {"Biryulyovo Passazhirskaya"s,55.580999, 37.659164} },
//		{ QueryType::BusInfo, {}, "256"s, {}, {} },
//		{ QueryType::BusInfo, {}, "750"s, {}, {} },
//		{ QueryType::BusInfo, {}, "751"s, {}, {} },
//		{ QueryType::Invalid, {}, {}, {}, {} },
//		{ QueryType::Invalid, {}, {}, {}, {}},
//		{ QueryType::StopInfo, "Biryulyovo Passazhirskaya"s, {}, {}, {} },
//		{ QueryType::StopInfo, "Universam"s, {}, {}, {} }
//	};
//	
//	
//	std::vector<Query> result;
//	{
//		InputReader input_reader;
//		for (size_t i = 0; i < 17; ++i) {
//			result.push_back(input_reader.GetQuery(iss));
//		}
//	}
//
//	assert(queries_ethalon == result);
//	std::cout << "InputReaderInputOperatorTest is OK"s << std::endl;
//	
//}

void InputReaderInputOperatorTest() {

	std::istringstream iss(
		
		"Stop Marushkino : 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\n"s
		"Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"s
		"Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\n"s
		"Stop Rasskazovka : 55.632761, 37.333324, 9500m to Marushkino\n"s
		"Stop Biryulyovo Zapadnoye : 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"s
		"Stop Biryusinka : 55.581065, 37.64839, 750m to Universam\n"s
		"Stop Universam : 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"s
		"Stop Biryulyovo Tovarnaya : 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"s
		"Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"s
		"Bus 828 : Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"s
		"Stop Rossoshanskaya ulitsa : 55.595579, 37.605757\n"s
		"Stop Prazhskaya : 55.611678, 37.603831\n"s		
		"Bus 256\n"s
		"Bus 750\n"s
		"Bus 751\n"s
		"Stop Samara\n"s
		"Stop Prazhskaya\n"s
		"Stop Biryulyovo Zapadnoye\n"s
		"Stap Biryulyovo Zapadnoye\n"s
		"Bas 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka"s
	);


	//struct Query {
	//	QueryType type;
	//	std::string stop_name_info;
	//	std::string bus_name_info;
	//	BusNew bus_new;
	//	Stop stop;
	//	std::vector<std::pair<std::string, int>> stop_distancies;
	//};


	std::vector<Query> queries_ethalon = {
		{ QueryType::AddStop, {}, {}, {}, {"Marushkino"s,55.595884, 37.209755}, { {"Rasskazovka"s, 9900},  {"Marushkino"s, 100} } },
		{ QueryType::AddBus, {}, {}, BusNew{"256"s, {"Biryulyovo Zapadnoye"s, "Biryusinka"s, "Universam"s, "Biryulyovo Tovarnaya"s, "Biryulyovo Passazhirskaya"s, "Biryulyovo Zapadnoye"s} },{}, {} },
		{ QueryType::AddBus, {}, {}, BusNew{"750"s, {"Tolstopaltsevo"s, "Marushkino"s, "Marushkino"s, "Rasskazovka"s, "Marushkino"s, "Marushkino"s, "Tolstopaltsevo"s} }, {}, {} },
		{ QueryType::AddStop, {}, {}, {}, {"Rasskazovka"s,55.632761, 37.333324}, { {"Marushkino"s, 9500} } },
		{ QueryType::AddStop, {}, {}, {}, {"Biryulyovo Zapadnoye"s,55.574371, 37.651700},  { {"Rossoshanskaya ulitsa"s, 7500},  {"Biryusinka"s, 1800}, {"Universam"s, 2400} } },
		{ QueryType::AddStop, {}, {}, {}, {"Biryusinka"s,55.581065, 37.648390},  { {"Universam"s, 750} } },
		{ QueryType::AddStop, {}, {}, {}, {"Universam"s,55.587655, 37.645687}, { {"Rossoshanskaya ulitsa"s, 5600},  {"Biryulyovo Tovarnaya"s, 900} } },
		{ QueryType::AddStop, {}, {}, {}, {"Biryulyovo Tovarnaya"s,55.592028, 37.653656},  { {"Biryulyovo Passazhirskaya"s, 1300} } },
		{ QueryType::AddStop, {}, {}, {}, {"Biryulyovo Passazhirskaya"s,55.580999, 37.659164}, { {"Biryulyovo Zapadnoye"s, 1200} } },
		{ QueryType::AddBus, {}, {}, BusNew{"828"s, {"Biryulyovo Zapadnoye"s, "Universam"s, "Rossoshanskaya ulitsa"s, "Biryulyovo Zapadnoye"s} },{}, {} },
		{ QueryType::AddStop, {}, {}, {}, {"Rossoshanskaya ulitsa"s, 55.595579, 37.605757}, {} },
		{ QueryType::AddStop, {}, {}, {}, {"Prazhskaya"s, 55.611678, 37.603831}, {} },
		{ QueryType::BusInfo, {}, "256"s, {}, {}, {} },
		{ QueryType::BusInfo, {}, "750"s, {}, {}, {} },
		{ QueryType::BusInfo, {}, "751"s, {}, {}, {} },
		{ QueryType::StopInfo, {"Samara"s}, {}, {}, {}, {} },
		{ QueryType::StopInfo, {"Prazhskaya"s}, {}, {}, {}, {} },
		{ QueryType::StopInfo, {"Biryulyovo Zapadnoye"s}, {}, {}, {}, {} },
		{ QueryType::Invalid, {}, {}, {}, {}, {} },
		{ QueryType::Invalid, {}, {}, {}, {}, {} },
	};


	std::vector<Query> result;
	{
		InputReader input_reader;
		for (size_t i = 0; i < queries_ethalon.size(); ++i) {
			result.push_back(input_reader.GetQuery(iss));
		}
	}

	assert(queries_ethalon == result);
	std::cout << "InputReaderInputOperatorTest is OK"s << std::endl;

}





void InputReaderRunTests() {
	//InputReaderGetCmdTest();
	//InputReaderInputOperatorTest();
	//InputReaderParseBusNameTest();
	//InputReaderParseBusRouteTest();
	InputReaderInputOperatorTest();

}


//void TransportCatalogueMethodsForStopTest() {
//	using namespace std::literals;
//	
//	std::vector<Stop> stops = {
//		{ "Airport"s, {52.525354, 25.252627} },
//		{ "Novokuznetsovo"s, {47.474849, 35.353637} },
//		{ "Abzakovo"s, {23.232425, 29.293031} },
//		{ "Staleleteinaya"s, {18.181920, 11.111213} },
//		{ "Pokrovskaya"s, {20.202122, 56.565758} },
//		{ "Marushkino"s, {55.595884, 37.209755} },
//		{ "Tolstopaltsevo"s, {55.611087, 37.208290} },
//		{ "Rasskazovka"s, {55.632761, 37.333324} },
//		{ "Biryulyovo Zapadnoye"s, {55.574371, 37.651700} },
//		{ "Universam"s, {55.587655, 37.645687} },
//		{ "Biryulyovo Tovarnaya"s, {55.592028, 37.653656} },
//		{ "Biryulyovo Passazhirskaya"s, {55.580999, 37.659164} }
//	};
//
//	TransportCatalogue transport_catalogue;
//	for (const Stop& stop : stops) {
//		transport_catalogue.AddStop(stop);
//	}
//
//	// existable stops
//	for (const Stop& stop : stops) {
//		const auto curr_stop = transport_catalogue.FindStop(stop.name);
//		assert( StopIsValid(curr_stop) );
//		assert(curr_stop == stop);
//	}
//
//	// nonexistable stops	
//	assert( StopIsValid (transport_catalogue.FindStop("Unavailable stop"s) ) == false );
//	assert( StopIsValid (transport_catalogue.FindStop("Another unavailable stop"s) ) == false);
//}


void TransportCatalogueAddBusTest() {
	using namespace std::literals;

	std::vector<Stop> stops = {
		{ "Airport"s, {52.525354, 25.252627} },
		{ "Novokuznetsovo"s, {47.474849, 35.353637} },
		{ "Abzakovo"s, {23.232425, 29.293031} },
		{ "Staleleteinaya"s, {18.181920, 11.111213} },
		{ "Pokrovskaya"s, {20.202122, 56.565758} },
		{ "Marushkino"s, {55.595884, 37.209755} },
		{ "Tolstopaltsevo"s, {55.611087, 37.208290} },
		{ "Rasskazovka"s, {55.632761, 37.333324} },
		{ "Biryulyovo Zapadnoye"s, {55.574371, 37.651700} },
		{ "Universam"s, {55.587655, 37.645687} },
		{ "Biryulyovo Tovarnaya"s, {55.592028, 37.653656} },
		{ "Biryulyovo Passazhirskaya"s, {55.580999, 37.659164} }
	};
	// old style AddBus
	{
		TransportCatalogue transport_catalogue;
		for (const Stop& stop : stops) {
			transport_catalogue.AddStop(stop);
		}
		// success operations
		assert(transport_catalogue.AddBus("811"s, { "Abzakovo"s, "Rasskazovka"s, "Marushkino"s, "Staleleteinaya"s, "Novokuznetsovo"s }));
		assert(transport_catalogue.AddBus("100"s, { "Biryulyovo Zapadnoye"s, "Marushkino"s, "Universam"s, "Biryulyovo Passazhirskaya"s }));

		// none success operations
		assert(!transport_catalogue.AddBus("999"s, { "unexistable stop"s, "church"s, "Universam"s, "Biryulyovo Passazhirskaya"s }));
		assert(!transport_catalogue.AddBus("111"s, { "Abzakovo"s, "Rasskazovka"s, "Marushkino"s, "another unexistable stop"s, ""s, ""s, "Biryulyovo Passazhirskaya"s }));
	}


	// new style AddBus
	{
		TransportCatalogue transport_catalogue;
		for (const Stop& stop : stops) {
			transport_catalogue.AddStop(stop);
		}
		// success operations
		assert(transport_catalogue.AddBus(BusNew{ "811"s, { "Abzakovo"s, "Rasskazovka"s, "Marushkino"s, "Staleleteinaya"s, "Novokuznetsovo"s } }));
		assert(transport_catalogue.AddBus(BusNew{ "100"s, { "Biryulyovo Zapadnoye"s, "Marushkino"s, "Universam"s, "Biryulyovo Passazhirskaya"s } }));

		// none success operations
		assert(!transport_catalogue.AddBus(BusNew{ "999"s, { "unexistable stop"s, "church"s, "Universam"s, "Biryulyovo Passazhirskaya"s } }));
		assert(!transport_catalogue.AddBus(BusNew{ "111"s, { "Abzakovo"s, "Rasskazovka"s, "Marushkino"s, "another unexistable stop"s, ""s, ""s, "Biryulyovo Passazhirskaya"s } }));
	}

	std::cout << "TransportCatalogueAddBusTest is OK"s << std::endl;

}


void TestGetBusesForStop() {

	using namespace std::literals;

	std::vector<Stop> stops = {
		{ "Airport"s, {52.525354, 25.252627} },
		{ "Novokuznetsovo"s, {47.474849, 35.353637} },
		{ "Abzakovo"s, {23.232425, 29.293031} },
		{ "Staleleteinaya"s, {18.181920, 11.111213} },
		{ "Pokrovskaya"s, {20.202122, 56.565758} },
		{ "Marushkino"s, {55.595884, 37.209755} },
		{ "Tolstopaltsevo"s, {55.611087, 37.208290} },
		{ "Rasskazovka"s, {55.632761, 37.333324} },
		{ "Biryulyovo Zapadnoye"s, {55.574371, 37.651700} },
		{ "Universam"s, {55.587655, 37.645687} },
		{ "Biryulyovo Tovarnaya"s, {55.592028, 37.653656} },
		{ "Biryulyovo Passazhirskaya"s, {55.580999, 37.659164} }
	};

	
	
	TransportCatalogue transport_catalogue;
	for (const Stop& stop : stops) {
		transport_catalogue.AddStop(stop);
	}
	// success operations
	transport_catalogue.AddBus(BusNew{ "811"s, { "Abzakovo"s, "Rasskazovka"s, "Marushkino"s, "Staleleteinaya"s, "Novokuznetsovo"s } });
	transport_catalogue.AddBus(BusNew{ "100"s, { "Biryulyovo Zapadnoye"s, "Marushkino"s, "Universam"s, "Biryulyovo Passazhirskaya"s } });
	{
		std::vector<std::string_view> ethalon = { "100"sv, "811"sv };
		assert(ethalon == transport_catalogue.GetBusesForStop("Marushkino"s).value());
	}
	{
		std::vector<std::string_view> ethalon = { "811"sv };
		assert(ethalon == transport_catalogue.GetBusesForStop("Abzakovo"s).value());
	}
	std::cout << "TestGetBusesForStop is OK"s << std::endl;
	

}



void TransportCatalogueRunTests() {
	//TransportCatalogueMethodsForStopTest();
	TransportCatalogueAddBusTest();
	TestGetBusesForStop();

}