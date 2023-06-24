#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include "geo.h"
#include <tuple>
#include <functional>
#include <optional>
#include <algorithm>
#include <numeric>

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

	void  SetStopsLength(std::string name_from, std::string name_to, double length);
	double GetStopsLength(const Stop& stop_from, const Stop& stop_to) const;
	double GetStopsGeoLength(const Stop& stop_from, const Stop& stop_to) const;

	void AddStop(const Stop& stop);
	const Stop& FindStop(std::string stop_name) const;
	bool AddBus(const std::string& bus_name, const std::vector<std::string>& route);	
	const Bus& FindBus(std::string bus_name) const;
	std::optional<BusInfo> GetBusInfo(std::string bus_name) const;
	std::optional<std::vector<std::string_view>> GetBusesForStop(const std::string& stop_name) const;	

private:

	template<typename Predicate>
	double ComputeRouteLength(const Bus& bus, Predicate predicate) const;

	static int GetUniqueStopsNum(const Bus& bus);
	std::deque<Stop> stops_;																			// структура данных содержащая остановки
	std::unordered_map<std::string_view, const Stop*, std::hash<std::string_view>> stopname_to_stop_;	// хеш таблица для быстрого поиска по структуре stops_
	std::deque<Bus> buses_;																				// структура данных, содержащая данные об автобусахы
	std::unordered_map<std::string_view, const Bus*, std::hash<std::string_view>> busname_to_bus_;		// хеш таблица для быстрого поиска по структуре buses_
	std::unordered_map<std::pair<const Stop*, const Stop*>, double, detailed::PairHasher> stop_to_stop_distances_;
	std::unordered_map<const Stop*, std::vector<const Bus*>, std::hash<const void*>> stop_to_buses_;
};


template<typename Predicate>
double TransportCatalogue::ComputeRouteLength(const Bus& bus, Predicate predicate) const {

	std::vector<double> distances(bus.route.size() - 1);
	std::transform(
		bus.route.begin(),
		std::prev(bus.route.end()),
		std::next(bus.route.begin()),
		distances.begin(),
		predicate
	);
	double distance = std::reduce(
		distances.begin(),
		distances.end(),
		0.0,
		std::plus<>()
	);
	return distance;
}

} // end of namespace cataloque

} // end of namespace transport