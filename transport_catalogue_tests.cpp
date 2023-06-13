
#include "transport_catalogue.h"
#include <cassert>



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