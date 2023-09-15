// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "json_reader.h"
#include <string_view>
#include <assert.h>
#include <iostream>



namespace transport{

namespace reader {


svg::Color JsonReader::GetSvgColor(const json::Node& source) const {
    if (source.IsArray()) {
        auto& raw_color = source.AsArray();
        if (raw_color.size() == 3) {
            return svg::Rgb{ 
                static_cast<uint8_t>( raw_color[0].AsInt() ), 
                static_cast<uint8_t>(raw_color[1].AsInt() ),
                static_cast<uint8_t>(raw_color[2].AsInt() ) 
            };
        }
        else if (raw_color.size() == 4) {
            return svg::Rgba{ 
                static_cast<uint8_t>( raw_color[0].AsInt() ),
                static_cast<uint8_t>( raw_color[1].AsInt() ),
                static_cast<uint8_t>( raw_color[2].AsInt() ),
                raw_color[3].AsDouble()
            };
        }
    }
    else if (source.IsString()) {
        return source.AsString();
    }
    return "none"s;
}

renderer::RenderSettings JsonReader::ExtractRenderSetts(const json::Dict& source) const {
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
            render_settings.color_palette.push_back( GetSvgColor(next_color) );
        }
        //q.render_settings.color_palette
    }
    catch (...)
    {
        std::cout << "Wrong render settings format"s;
    }
    return render_settings;
}

transport::requests::QueriesQueue JsonReader::Process() {
	transport::requests::QueriesQueue queries_queue_;
	json::Document document = json::Load(input_);
	json::Print(document, std::cout);
	for (const auto& request_type : document.GetRoot().AsMap()) {
		if (request_type.first == "base_requests"s || request_type.first == "stat_requests"s) {
			for (const auto& request : request_type.second.AsArray()) {
				auto q = ExtractQuery(request.AsMap());
				queries_queue_.Push( std::move(q) );				
			}
		}
		else if (request_type.first == "render_settings"s) {
			queries_queue_.SetRendererSettings( ExtractRenderSetts( request_type.second.AsMap() ) );
		}
	}
	return queries_queue_;
}

transport::requests::Query JsonReader::ExtractQuery(const json::Dict& source){	
	transport::requests::Query q{};

	if (source.at("type").AsString() == "Stop") {
		if (const auto it = source.find("id"); it != source.end()) {
			q.type = transport::requests::QueryType::StopInfo;
			q.id = it->second.AsInt();
			q.stop_name_info = source.at("name").AsString();
		}
		else {
			q.type = transport::requests::QueryType::AddStop;
			q.stop.name = source.at("name").AsString();
			q.stop.coordinates = { source.at("latitude").AsDouble(), source.at("longitude").AsDouble() };
			for (const auto& [stop_to, distance] : source.at("road_distances").AsMap()) {
				q.stop_distancies.push_back( { stop_to, distance.AsInt() } );
			}
		}		
	}
	else if (source.at("type"s).AsString() == "Bus") {
		if (const auto it = source.find("id"); it != source.end()) {
			q.type = transport::requests::QueryType::BusInfo;
			q.id = it->second.AsInt();
			q.bus_name_info = source.at("name").AsString();
		}
		else {
			q.type = transport::requests::QueryType::AddBus;
			q.busname_to_route.first = source.at("name").AsString();
			q.is_roundtrip = source.at("is_roundtrip").AsBool();
			const auto& stops = source.at("stops").AsArray();
			for (const auto& stop : stops ){
				q.busname_to_route.second.push_back(stop.AsString());
			}			
			
			if (!q.is_roundtrip) {
				auto& route = q.busname_to_route.second;
				route.resize(2 * route.size() - 1);
				std::copy(route.begin(), std::prev(route.end()), route.rbegin());
			}
			
		}
	}
	else if (source.at("type"s).AsString() == "Map") {
		q.type = transport::requests::QueryType::MapInfo;
		if (const auto it = source.find("id"); it != source.end()) {			
			q.id = it->second.AsInt();			
		}
	}
	return q;
}
//-----------------json reader end -----------------

} // end of namespace reader

} // end of namespace transport



