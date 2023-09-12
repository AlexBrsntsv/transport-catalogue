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
#include "domain.h"
#include <set>



namespace transport {

namespace catalogue {

namespace detailed {

struct PairHasher {	
	size_t operator()(const std::pair<const void*, const void*>& p) const {
		size_t prime_number = 37;
		return std::hash<const void*>{}(p.first) + std::hash<const void*>{}(p.second) * prime_number;
	}
};

} // end of namespace detailed

struct BusInfo {
	std::string name;
	int stops_on_route;
	int unique_stops;
	double route_length;
	double curvature;
};


class TransportCatalogue {

public:
	//using BusInfo = std::tuple<int, int, double, double>;

	void  SetStopsLength(std::string name_from, std::string name_to, double length);
	double GetStopsLength(const domain::Stop& stop_from, const domain::Stop& stop_to) const;
	double GetStopsGeoLength(const domain::Stop& stop_from, const domain::Stop& stop_to) const;

	//const auto& GetAllStops();
	const std::deque<domain::Bus>& GetAllBuses() const;
	std::set<std::string_view> GetBusesList() const;

	void AddStop(domain::Stop stop);
	const domain::Stop& FindStop(std::string stop_name) const;
	bool AddBus(const std::string& bus_name, const std::vector<std::string>& route);	
	const domain::Bus& FindBus(std::string bus_name) const;

	std::optional<BusInfo> GetBusInfo(std::string bus_name) const;
	std::optional<std::vector<std::string_view>> GetBusesForStop(const std::string& stop_name) const;	

private:

	template<typename Predicate>
	double ComputeRouteLength(const domain::Bus& bus, Predicate predicate) const;

	static int GetUniqueStopsNum(const domain::Bus& bus);
	std::deque<domain::Stop> stops_;																			// структура данных содержащая остановки
	std::unordered_map<std::string_view, const domain::Stop*, std::hash<std::string_view>> stopname_to_stop_;	// хеш таблица для быстрого поиска по структуре stops_
	std::deque<domain::Bus> buses_;																				// структура данных, содержащая данные об автобусахы
	std::unordered_map<std::string_view, const domain::Bus*, std::hash<std::string_view>> busname_to_bus_;		// хеш таблица для быстрого поиска по структуре buses_
	std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, double, detailed::PairHasher> stop_to_stop_distances_;
	std::unordered_map<const domain::Stop*, std::vector<const domain::Bus*>, std::hash<const void*>> stop_to_buses_;
};


template<typename Predicate>
double TransportCatalogue::ComputeRouteLength(const domain::Bus& bus, Predicate predicate) const {

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