#pragma once
#include "geo.h"
#include <string>
#include <vector>

namespace domain {


struct Stop {
	std::string name;
	geo::Coordinates coordinates;

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

const Stop invalid_stop = { "", {} };	// unexisted stop
const Bus invalid_bus = { "", {} };		// unexisted bus



} // end of namespace domain