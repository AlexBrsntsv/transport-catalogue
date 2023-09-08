// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "json_reader.h"
#include <string_view>
#include <assert.h>


using transport::reader::Query;
using transport::reader::QueryType;


namespace transport{

namespace reader {

size_t GetPriority(QueryType q) {
	size_t result = 0;
	switch (q) {
	case QueryType::AddStop:
		result = 0; // highest priority
		break;

	case QueryType::Distancies:
		result = 1;
		break;
	case QueryType::AddBus:
		result = 2;
		break;

	case QueryType::BusInfo: // have same priority
	case QueryType::StopInfo:
		result = 3;
		break;
	case QueryType::DrawSettings:
		result = 4;
		break;
	default:
		result = static_cast<size_t>(QueryType::QueryTypesAmount);
		break;
	}
	return result;
}

//---------  InputReader -----------
void InputReader::ProcessQuery(Query&& q) {
	if ((q.type == QueryType::AddStop) && !q.stop_distancies.empty()) {
		Query extra_q;
		extra_q.type = QueryType::Distancies;
		extra_q.stop = q.stop;
		extra_q.stop_distancies.swap(q.stop_distancies);
		queue_.Add(extra_q, GetPriority(extra_q.type));
	}
	queue_.Add(q, GetPriority(q.type));
}

Query InputReader::GetNext() {
	return (queue_.GetNext());
}
bool InputReader::Empty() const {
	return (queue_.Empty());
}

bool operator==(const Query& lhs, const Query& rhs) {
	return
		lhs.bus_name_info == rhs.bus_name_info &&
		lhs.busname_to_route.first == rhs.busname_to_route.first &&
		lhs.busname_to_route.second == rhs.busname_to_route.second &&
		lhs.stop == rhs.stop &&
		lhs.type == rhs.type;
}



void QueryTypeHandler(
	Query&& q, 
	transport::catalogue::TransportCatalogue& db, 
	transport::statistics::StatisticsBaseOutput* statistics_output
) {
	
	switch (q.type) {
	case QueryType::AddStop:
		db.AddStop(q.stop);
		break;
	case QueryType::AddBus:
		db.AddBus(q.busname_to_route.first, q.busname_to_route.second);
		break;
	case QueryType::Distancies:
		for (const auto& [to_stop, distance] : q.stop_distancies) {
			db.SetStopsLength(q.stop.name, to_stop, distance);
		}
		break;	
	case QueryType::StopInfo:
		if (const auto& stop_info = db.GetBusesForStop(q.stop_name_info); stop_info) {
			statistics_output->Add(q.stop_name_info, *stop_info, q.id);
		}
		else {
			statistics_output->AddError(q.stop_name_info, statistics::RequestError::Stop, q.id);
		}
		break;
	case QueryType::BusInfo:
		if (const auto& bus_info = db.GetBusInfo(q.bus_name_info); bus_info) {
			statistics_output->Add( *bus_info, q.id);
		}
		else {
			statistics_output->AddError(q.bus_name_info, statistics::RequestError::Bus, q.id);
		}		
		break;
        
        case QueryType::QueryTypesAmount:
        break;
        case QueryType::Invalid:
        break;
        
	}
	if (statistics_output->Ready()) {
		statistics_output->Show();
	}
}

//---------------- json_reader -------------------
void JsonReader::Process(size_t) {
	json::Document document = json::Load(input_);
	for (const auto& request_type : document.GetRoot().AsMap()) {
		if (request_type.first == "base_requests"s || request_type.first == "stat_requests"s) {
			for (const auto& request : request_type.second.AsArray()) {
				ProcessQuery(ExtractQuery(request.AsMap()));
			}
		}
		else if (request_type.first == "render_settings"s) {
			auto q = ExtractQuery(request_type.second.AsMap());
		}
	}
}


Query JsonReader::ExtractQuery(const json::Dict& source){
	Query q{};
	if (auto mark = source.find("type"); mark == source.end()) {
		q.type = QueryType::DrawSettings;
		q.render_settings.width = source.at("width").AsDouble();
		q.render_settings.height = source.at("height").AsDouble();
		q.render_settings.padding = source.at("padding").AsDouble();
		q.render_settings.line_width = source.at("line_width").AsDouble();
		q.render_settings.stop_radius = source.at("stop_radius").AsDouble();
		q.render_settings.bus_label_font_size = source.at("bus_label_font_size").AsInt();
		q.render_settings.bus_label_offset.first = source.at("bus_label_offset").AsArray().at(0).AsDouble();
		q.render_settings.bus_label_offset.second = source.at("bus_label_offset").AsArray().at(1).AsDouble();
		q.render_settings.stop_label_font_size = source.at("stop_label_font_size").AsInt();
		q.render_settings.stop_label_offset.first = source.at("stop_label_offset").AsArray().at(0).AsDouble();
		q.render_settings.stop_label_offset.second = source.at("stop_label_offset").AsArray().at(1).AsDouble();
		//q.render_settings.underlayer_color
		q.render_settings.underlayer_width = source.at("underlayer_width").AsDouble();
		//q.render_settings.color_palette



	}
	else if (source.at("type").AsString() == "Stop") {
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
//-----------------json reader end -----------------











} // end of namespace reader

void RequestsProcess(
	transport::reader::InputReader* reader,
	int query_num,
	transport::catalogue::TransportCatalogue& db,
	transport::statistics::StatisticsBaseOutput* statistics_output
) {
	reader->Process(query_num);
	while (!reader->Empty()) {
		QueryTypeHandler(reader->GetNext(), db, statistics_output);
	}
	statistics_output->Show();
}

} // end of namespace transport



