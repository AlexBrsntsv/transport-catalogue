#include "request_handler.h"
#include "json_reader.h"
#include <sstream>


/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */


// напишите решение с нуля
// код сохраните в свой git-репозиторий

#include "request_handler.h"
#include <math.h>


using transport::catalogue::BusInfo;
using namespace std::literals;

namespace transport {

namespace requests {

bool operator==(const Query& lhs, const Query& rhs) {
	return
		lhs.bus_name_info == rhs.bus_name_info &&
		lhs.busname_to_route.first == rhs.busname_to_route.first &&
		lhs.busname_to_route.second == rhs.busname_to_route.second &&
		lhs.stop == rhs.stop &&
		lhs.type == rhs.type;
}

const renderer::RenderSettings& QueriesQueue::GetRendererSettings() const{
	return render_settings_;
}

const renderer::RenderSettings& QueriesQueue::SetRendererSettings(renderer::RenderSettings&& r_setts) {
	render_settings_ = std::move(r_setts);

}
const Query& QueriesQueue::Front(QueryType q_type) {
	static Query invalid_q;

	switch (q_type) {
	case QueryType::AddBus:
		return add_bus_queries_.front();
		break;
	case QueryType::AddStop:
		return add_stop_queries_.front();
		break;
	case QueryType::Distancies:
		return add_distance_queries_.front();
		break;
	case QueryType::Info:
		return info_queries_.front();
		break;

	default:
		return invalid_q;
		break;
	}

}
void QueriesQueue::Pop(QueryType q_type) {
	switch (q_type) {
	case QueryType::AddBus:
		add_bus_queries_.pop();
		break;
	case QueryType::AddStop:
		add_stop_queries_.pop();
		break;
	case QueryType::Distancies:
		add_distance_queries_.pop();
		break;
	case QueryType::Info:
		info_queries_.pop();
		break;

	default:
		throw (std::invalid_argument("invalid query type"));
		break;
	}
}

bool QueriesQueue::Empty(QueryType q_type) const {
	switch (q_type) {
	case QueryType::AddBus:
		return add_bus_queries_.empty();
		break;
	case QueryType::AddStop:
		return add_stop_queries_.empty();
		break;
	case QueryType::Distancies:
		return add_distance_queries_.empty();
		break;
	case QueryType::Info:
		return info_queries_.empty();
		break;

	default:
		throw (std::invalid_argument("invalid query type"));
		break;
	}
}

void QueriesQueue::Push(Query&& q) {
	switch (q.type) {
	case QueryType::AddBus:
		add_bus_queries_.push(std::move(q));
		break;
	case QueryType::AddStop:
		add_stop_queries_.push(std::move(q));
		break;
	case QueryType::Distancies:
		add_distance_queries_.push(std::move(q));
		break;
	case QueryType::BusInfo:
	case QueryType::StopInfo:
	case QueryType::MapInfo:
		info_queries_.push(std::move(q));
		break;

	default:
		throw (std::invalid_argument("invalid query type"));
		break;
	}
}

void ProccessInputTypeQueries(transport::catalogue::TransportCatalogue& db, transport::requests::QueriesQueue& q) {	
	while (!q.Empty(QueryType::AddStop)) {
		db.AddStop(q.Front(QueryType::AddStop).stop);
		q.Pop(QueryType::AddStop);
	}
	while (!q.Empty(QueryType::AddBus)) {
		auto query = q.Front(QueryType::AddBus);
		db.AddBus(query.busname_to_route.first, query.busname_to_route.second, query.is_roundtrip);
		q.Pop(QueryType::AddBus);
	}
}

void RequestHandler::Proccess(QueriesQueue& q_query, requests::StatisticsBaseOutput& output_format){

	while ( !q_query.Empty(QueryType::Info) ) {
		auto q = q_query.Front(QueryType::Info);
		switch (q.type) {
		case QueryType::StopInfo:
			if (const auto& stop_info = db_.GetBusesForStop(q.stop_name_info); stop_info) {
				output_format.Add(q.stop_name_info, *stop_info, q.id);
			}
			else {
				output_format.AddError(q.stop_name_info, requests::RequestError::Stop, q.id);
			}
			break;
		case QueryType::BusInfo:
			if (const auto& bus_info = db_.GetBusInfo(q.bus_name_info); bus_info) {
				output_format.Add(*bus_info, q.id);
			}
			else {
				output_format.AddError(q.bus_name_info, requests::RequestError::Bus, q.id);
			}
			break;


		case QueryType::MapInfo:
			output_format.AddMap(db_, renderer_);

		}

		q_query.Pop(QueryType::Info);
	}




}

	
void StatisticsTextOutput::Add(const BusInfo& bus_info, int) {
	double route_length = std::round(bus_info.route_length * 100) / 100;
	buffer_.push(
		"Bus "s + bus_info.name + ": "s +
		std::to_string(bus_info.stops_on_route) + " stops on route, "s +
		std::to_string(bus_info.unique_stops) + " unique stops, " +
		std::to_string(static_cast<int>(route_length)) + " route length, "s +
		std::to_string(bus_info.curvature) + " curvature"s
	);
}

void StatisticsTextOutput::Add(const std::string& stop_name, std::vector<std::string_view> buses, int) {
	std::string result;
	if (buses.empty()) {
		result = "Stop "s + stop_name + ": no buses"s;
	}
	else {
		result = "Stop "s + stop_name + ": buses"s;
		for (std::string_view bus : buses) {
			result += ' ';
			result += std::string(bus);
		}
	}
	buffer_.push (result);
}

void StatisticsTextOutput::AddError(std::string name, RequestError error_mark, int) {
	std::string result;
	if (error_mark == RequestError::Stop) {
		result =  "Stop "s + name + ": not found"s;
	}
	else if (error_mark == RequestError::Bus) {
		result = "Bus "s + name + ": not found"s;
	}
	buffer_.push(result);
}

void StatisticsTextOutput::Show() {
	while (!buffer_.empty()) {
		output_ << buffer_.front() << std::endl;
		buffer_.pop();
	}
}

bool StatisticsTextOutput::Ready() {
	return !buffer_.empty();
}


void StatisticsJsonOutput::Add(const transport::catalogue::BusInfo& bus_info, int id){	
	json::Dict node_to_add{
		{"curvature", bus_info.curvature},
		{"request_id" , id},
		{"route_length" , bus_info.route_length},
		{"stop_count", bus_info.stops_on_route},
		{"unique_stop_count", bus_info.unique_stops}
	};
	buffer_.emplace_back(json::Node(node_to_add));
}

void StatisticsJsonOutput::Add(const std::string&, std::vector<std::string_view> buses, int id) {
	json::Array buses_list;
	std::transform(
		buses.begin(),
		buses.end(),
		std::back_inserter(buses_list),
		[](const auto& element) { return json::Node(std::string(element)); }
	);

	json::Dict node_to_add{
		{"buses", buses_list},
		{"request_id" , id}
	};
	buffer_.emplace_back(json::Node(node_to_add));
}


void StatisticsJsonOutput::AddError(std::string, RequestError, int id){
	json::Dict node_to_add{
		{"request_id"s, id},
		{"error_message"s , "not found"s}
	};
	buffer_.emplace_back( json::Node(node_to_add) );
}

void StatisticsJsonOutput::Show() {
	output_ << json::Node(buffer_);
}
bool StatisticsJsonOutput::Ready() { return false; }

//std::vector<geo::Coordinates> GetStopsCoordinatesForBuses(const transport::catalogue::TransportCatalogue& db) {
//	std::vector<geo::Coordinates> result;
//	for (const domain::Bus& bus : db.GetAllBuses()) {
//		for (const auto& stop : bus.route) {
//			result.push_back(stop->coordinates);
//		}
//	}
//	return result;
//}

/*
svg::Color GetSvgColor(const json::Node& source) {
	if (source.IsArray()) {
		auto& raw_color = source.AsArray();
		if (raw_color.size() == 3) {
			return svg::Rgb{
				static_cast<uint8_t>(raw_color[0].AsInt()),
				static_cast<uint8_t>(raw_color[1].AsInt()),
				static_cast<uint8_t>(raw_color[2].AsInt())
			};
		}
		else if (raw_color.size() == 4) {
			return svg::Rgba{
				static_cast<uint8_t>(raw_color[0].AsInt()),
				static_cast<uint8_t>(raw_color[1].AsInt()),
				static_cast<uint8_t>(raw_color[2].AsInt()),
				raw_color[3].AsDouble()
			};
		}
	}
	else if (source.IsString()) {
		return source.AsString();
	}
	return "none"s;
}
*/
/*
renderer::RenderSettings ExtractRenderSettings(const json::Dict& source) {
	renderer::RenderSettings render_settings;
	try {
		render_settings.width = source.at("width").AsDouble();
		render_settings.height = source.at("height").AsDouble();
		render_settings.padding = source.at("padding").AsDouble();
		render_settings.line_width = source.at("line_width").AsDouble();
		render_settings.stop_radius = source.at("stop_radius").AsDouble();
		render_settings.bus_label_font_size = source.at("bus_label_font_size").AsInt();
		render_settings.bus_label_offset.first = source.at("bus_label_offset").AsArray().at(0).AsDouble();
		render_settings.bus_label_offset.second = source.at("bus_label_offset").AsArray().at(1).AsDouble();
		render_settings.stop_label_font_size = source.at("stop_label_font_size").AsInt();
		render_settings.stop_label_offset.first = source.at("stop_label_offset").AsArray().at(0).AsDouble();
		render_settings.stop_label_offset.second = source.at("stop_label_offset").AsArray().at(1).AsDouble();
		render_settings.underlayer_color = GetSvgColor(source.at("underlayer_color"));
		render_settings.underlayer_width = source.at("underlayer_width").AsDouble();
		for (auto next_color : source.at("color_palette").AsArray()) {
			render_settings.color_palette.push_back(GetSvgColor(next_color));
		}
		//q.render_settings.color_palette
	}
	catch (...)
	{
		std::cout << "Wrong render settings format"s;
	}
	return render_settings;
}
*/


void StatisticsJsonOutput::AddMap(const transport::catalogue::TransportCatalogue& db, renderer::MapRenderer& map_renderer, int id) {
	std::ostringstream ss;
	std::set<std::string> unique_stop_names_;
	for (std::string_view busname : db.GetBusesList()) {
		const domain::Bus& next_bus = db.FindBus(std::string(busname));
		if (next_bus == domain::invalid_bus) {
			continue;
		}
		auto stops_for_bus = db.GetUniqueStopNamesForBus(next_bus);
		unique_stop_names_.insert(stops_for_bus.begin(), stops_for_bus.end());
		map_renderer.AddBus(next_bus);
	}
	for (const std::string& stop_name : unique_stop_names_) {
		const domain::Stop& next_stop = db.FindStop(stop_name);
		if (next_stop != domain::invalid_stop) {
			map_renderer.AddStop(next_stop);
		}
	}
	map_renderer.Render(ss);
	json::Dict node_to_add{
		{"map"s, ss.str()},
		{"id"s , id}
	};
	buffer_.emplace_back(json::Node(node_to_add));
}

} // end of namespace requests

} // end of namespace transport

