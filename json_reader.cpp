#include "json_reader.h"


using transport::reader::Query;
using transport::reader::QueryType;


namespace transport{

namespace reader {

void JsonReader::Process(size_t) {
	json::Document document = json::Load(input_);
	for (const auto& request_type : document.GetRoot().AsMap()) {
		for (const auto& request : request_type.second.AsArray()) {
			ProcessQuery( ExtractQuery( request.AsMap() ) );
		}
	}
}

Query JsonReader::ExtractQuery(const json::Dict& source){
	Query q{};
	if (source.at("type").AsString() == "Stop") {
		if (const auto it = source.find("id"); it != source.end()) {
			q.type = QueryType::StopInfo;
			q.id = it->second.AsInt();
			q.stop_name_info = source.at("name").AsString();
		}
		else {
			q.type = QueryType::AddStop;
			q.stop.name = source.at("name").AsString();
			q.stop.coordinates = { source.at("latitude").AsDouble(), source.at("longitude").AsDouble() };
			for (const auto& [stop_to, distance] : source.at("road_distances").AsMap()) {
				q.stop_distancies.push_back( { stop_to, distance.AsInt() } );
			}
		}		
	}
	else if (source.at("type"s).AsString() == "Bus") {
		if (const auto it = source.find("id"); it != source.end()) {
			q.type = QueryType::BusInfo;
			q.id = it->second.AsInt();
			q.bus_name_info = source.at("name").AsString();
		}
		else {
			q.type = QueryType::AddBus;
			q.busname_to_route.first = source.at("name").AsString();
			bool is_round_trip = source.at("is_roundtrip").AsBool();
			const auto& stops = source.at("stops").AsArray();
			for (const auto& stop : stops ){
				q.busname_to_route.second.push_back(stop.AsString());
			}
			if (!is_round_trip) {
				auto& route = q.busname_to_route.second;
				route.resize(2 * route.size() - 1);
				std::copy(route.begin(), std::prev(route.end()), route.rbegin());
			}
		}
	}

	return q;
}


} //end of namespace reader


} // end of namespace transport

//{
//	"stat_requests": []
//	"base_requests" : []
//}

