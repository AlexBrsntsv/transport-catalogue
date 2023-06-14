
#include "tests.h"
#include "transport_catalogue.h"
#include "input_reader.h"
#include <cassert>
#include <algorithm>







//===============================================================
// input_reader tests




void InputReaderGetCmdTest() {
	std::vector<std::string> strings_to_parse = {
		"Stop Tolstopaltsevo : 55.611087, 37.208290"s,
		"Stop Marushkino : 55.595884, 37.209755"s,
		"Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s,
		"Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka"s,
		"Stop Rasskazovka : 55.632761, 37.333324"s,
		"Stop   Biryulyovo Zapadnoye : 55.574371, 37.651700"s,
		"Stop Biryusinka : 55.581065, 37.648390"s,
		"Stop Universam : 55.587655, 37.645687"s,
		"Stop Biryulyovo Tovarnaya : 55.592028, 37.653656"s,
		"Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164"s,
		"Bus 256"s,
		"Bus 750"s,
		"Bus    751"s,
		"SBus    751"s,
		"Stap Biryulyovo Passazhirskaya : 55.580999, 37.659164"s,

	};

	const std::vector<QueryType> comands_type_ethalon = {
		QueryType::AddStop,				
		QueryType::AddStop,
		QueryType::AddBus,
		QueryType::AddBus,
		QueryType::AddStop,
		QueryType::AddStop,
		QueryType::AddStop,
		QueryType::AddStop,
		QueryType::AddStop,
		QueryType::AddStop,
		QueryType::BusInfo,
		QueryType::BusInfo,
		QueryType::BusInfo,
		QueryType::Unknown,
		QueryType::Unknown
	};

	std::vector<QueryType> comands_type_real;
	for (const std::string& s : strings_to_parse) {
		comands_type_real.push_back( GetQueryType(s) );
	}
	assert(comands_type_real == comands_type_ethalon);
}






void InputReaderRunTests() {
	InputReaderGetCmdTest();

}













void TransportCatalogueMethodsForStopTest() {
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

	// existable stops
	for (const Stop& stop : stops) {
		const auto curr_stop = transport_catalogue.FindStop(stop.name);
		assert( StopIsValid(curr_stop) );
		assert(curr_stop == stop);
	}

	// nonexistable stops	
	assert( StopIsValid (transport_catalogue.FindStop("Unavailable stop"s) ) == false );
	assert( StopIsValid (transport_catalogue.FindStop("Another unavailable stop"s) ) == false);
}


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


	





	PrintBus( transport_catalogue.FindBus("811"s) );
	PrintBus( transport_catalogue.FindBus("100"s));
	PrintBus(transport_catalogue.FindBus("999"s));
	PrintBus(transport_catalogue.FindBus("111"s));



}


void TransportCatalogueRunTests() {
	TransportCatalogueMethodsForStopTest();
	TransportCatalogueAddBusTest();

}