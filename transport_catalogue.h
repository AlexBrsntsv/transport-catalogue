#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include "geo.h"
#include <tuple>
#include <functional>
#include <optional>



struct Stop {
	std::string name;
	Coordinates coordinates;	

	bool operator==(const Stop& other) const {
		return coordinates == other.coordinates && name == other.name;
	}
	bool operator!=(const Stop& other) const {
		return !(*this == other);
	}
};
bool StopIsValid(const Stop& stop);


struct Bus {
	std::string name;
	std::vector<const Stop*> route;

	bool operator==(const Bus& other) const {
		return route == other.route && name == other.name;
	}
	bool operator!=(const Bus& other) const {
		return !(*this == other);
	}
};
bool BusIsValid(const Bus& bus);
void PrintBus(const Bus& bus);


struct PairHasher {	
	size_t operator()(const std::pair<Stop*, Stop*>& p) {
		size_t prime_number = 37;
		return reinterpret_cast<size_t>(p.first) + reinterpret_cast<size_t>(p.second) * prime_number;
	}
};


class TransportCatalogue {
	using BusInfo = std::tuple<int, int, double>;
	
public:
	void AddStop(const Stop& stop);

	const Stop& FindStop(std::string stop_name) const;

	bool AddBus(const std::string& bus_name, const std::vector<std::string>& stops_list );
	const Bus& FindBus(std::string bus_name) const;
	std::optional<BusInfo> GetBusInfo(std::string bus_name) const;


private:
	std::deque<Stop> stops_;																			// структура данных содержащая остановки
	std::unordered_map<std::string_view, const Stop*, std::hash<std::string_view>> stopname_to_stop_;	// хеш таблица для быстрого поиска по структуре stops_
	std::deque<Bus> buses_;																				// структура данных, содержащая данные об автобусахы
	std::unordered_map<std::string_view, const Bus*, std::hash<std::string_view>> busname_to_bus_;		// хеш таблица для быстрого поиска по структуре buses_
	std::unordered_map<std::pair<Stop*, Stop*>, double, PairHasher> stop_to_stop_distances_;
};