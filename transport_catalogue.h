#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include "geo.h"
#include <tuple>
#include <functional>
#include <optional>

namespace transport {

namespace catalogue {

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

struct BusNew {
	std::string name;
	std::vector<std::string> route;
};

namespace detailed {

struct PairHasher {
	size_t operator()(const std::pair<const Stop*, const Stop*>& p) const {
		size_t prime_number = 37;
		return std::hash<const void*>{}(p.first) + std::hash<const void*>{}(p.second) * prime_number;
	}
};

} // end of namespace detailed

class TransportCatalogue {

public:
	using BusInfo = std::tuple<int, int, double, double>;

	void  AddStopsLength(std::string name_from, std::string name_to, double length);
	double GetStopsLength(const Stop& stop_from, const Stop& stop_to) const;
	double GetStopsGeoLength(const Stop& stop_from, const Stop& stop_to) const;

	void AddStop(const Stop& stop);
	const Stop& FindStop(std::string stop_name) const;
	bool AddBus(const std::string& bus_name, const std::vector<std::string>& stops_list);
	bool AddBus(const BusNew& bus);
	const Bus& FindBus(std::string bus_name) const;
	std::optional<BusInfo> GetBusInfo(std::string bus_name) const;
	std::optional<std::vector<std::string_view>> GetBusesForStop(const std::string& stop_name) const;


	double CalculateRouteLengthViaCoordinates(const Bus& bus) const;
	double CalculateRouteLengthViaLengths(const Bus& bus) const;


private:

	static int GetUniqueStopsNum(const Bus& bus);
	std::deque<Stop> stops_;																			// структура данных содержащая остановки
	std::unordered_map<std::string_view, const Stop*, std::hash<std::string_view>> stopname_to_stop_;	// хеш таблица для быстрого поиска по структуре stops_
	std::deque<Bus> buses_;																				// структура данных, содержащая данные об автобусахы
	std::unordered_map<std::string_view, const Bus*, std::hash<std::string_view>> busname_to_bus_;		// хеш таблица для быстрого поиска по структуре buses_
	std::unordered_map<std::pair<const Stop*, const Stop*>, double, detailed::PairHasher> stop_to_stop_distances_;
	std::unordered_map<const Stop*, std::vector<const Bus*>, std::hash<const void*>> stop_to_buses_;
};

} // end of namespace cataloque

} // end of namespace transport