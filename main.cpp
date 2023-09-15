#include <iostream>
//#include "tests.h"
#include <sstream>
#include <iomanip>
//#include "input_reader.h"
#include "json_reader.h"
#include "svg.h"

#include "transport_catalogue.h"
#include <sstream>
#include "json.h"
using namespace std;
using namespace transport;
using namespace renderer;

int ReadNumber(std::istream& is) {		
	std::string s;
	std::getline(is, s);
	return std::stoi(s);
}

std::vector<geo::Coordinates> GetStopsCoordinatesForBuses(const transport::catalogue::TransportCatalogue& db) {
    std::vector<geo::Coordinates> result;
    for (const domain::Bus& bus : db.GetAllBuses()) {
        for (const auto& stop : bus.route) {
            result.push_back(stop->coordinates);
        }
    }
    return result;
}
//
//
//svg::Color GetSvgColor(const json::Node& source) {
//    if (source.IsArray()) {
//        auto& raw_color = source.AsArray();
//        if (raw_color.size() == 3) {
//            return svg::Rgb{ 
//                static_cast<uint8_t>( raw_color[0].AsInt() ), 
//                static_cast<uint8_t>(raw_color[1].AsInt() ),
//                static_cast<uint8_t>(raw_color[2].AsInt() ) 
//            };
//        }
//        else if (raw_color.size() == 4) {
//            return svg::Rgba{ 
//                static_cast<uint8_t>( raw_color[0].AsInt() ),
//                static_cast<uint8_t>( raw_color[1].AsInt() ),
//                static_cast<uint8_t>( raw_color[2].AsInt() ),
//                raw_color[3].AsDouble()
//            };
//        }
//    }
//    else if (source.IsString()) {
//        return source.AsString();
//    }
//    return "none"s;
//}



int main() {	
	//transport::tests::Catalogue();
	//transport::tests::Input();  
    std::setlocale(LC_ALL, "Russian");
    std::cout << "Русский язык!"s << std::endl;

	
	
  
    
    std::setlocale(LC_NUMERIC, "en_US.UTF-8"); // чтобы std::stod работала корректно
    
    
    std::istringstream ss(
        " {"s
        "          \"base_requests\": ["s
        "{"s
        "  \"type\": \"Bus\","s
        "  \"name\" : \"114\","s
        " \"stops\" : [\"Морской вокзал\", \"Ривьерский мост\"] ,"s
        "  \"is_roundtrip\" : false"s
        " },"s
        " {"s
        "\"type\": \"Stop\","s
        "\"name\" : \"Ривьерский мост\","s
        " \"latitude\" : 43.587795,"s
        " \"longitude\" : 39.716901,"s
        "\"road_distances\" : {\"Морской вокзал\": 850}"
        "},"s
        "{"s
        " \"type\": \"Stop\","s
        "\"name\" : \"Морской вокзал\","
        "\"latitude\" : 43.581969,"
        "\"longitude\" : 39.719848,"
        "\"road_distances\" : {\"Ривьерский мост\": 850}"s
        "}"s
        "],"s
        "\"stat_requests\": ["s
        " { \"id\": 1, \"type\" : \"Stop\", \"name\" : \"Ривьерский мост\" },"s
        " { \"id\": 2, \"type\" : \"Bus\", \"name\" : \"114\" }"s
        "]"s
        "}"s
    );  


    //std::istringstream ss(
    //    
    //    " {"s
    //    "          \"render_settings\": "s
    //    "{"s
    //    "\"width\": 1200.0,"s
    //    " \"height\" : 1200.0,"s
    //    " \"padding\" : 50.0,"s
    //    " \"line_width\" : 14.0,"s
    //    "  \"stop_radius\" : 5.0,"s

    //    " \"bus_label_font_size\" : 20,"s
    //    "\"bus_label_offset\" : [7.0, 15.0] ,"s

    //    " \"stop_label_font_size\" : 20,"s
    //    "\"stop_label_offset\" : [7.0, -3.0] ,"s

    //    "\"underlayer_color\" : [253, 254, 255, 0.85] ,"s
    //    "\"underlayer_width\" : 3.0,"s

    //    " \"color_palette\" : ["s
    //    " \"green\","s
    //    " [255, 160, 0],"s
    //    " \"red\""s
    //    " ]"
    //    "}"s
    //    " }"s
    //    

    //);
        
    
    transport::catalogue::TransportCatalogue transport_catalogue;    
    //transport::reader::JsonReader json_reader(std::cin);
    transport::reader::JsonReader json_reader(ss);
    transport::requests::QueriesQueue q = json_reader.Process();
    // send data to transport catalogue
    transport::requests::ProccessInputTypeQueries(transport_catalogue, q);
    auto coordinates_array = GetStopsCoordinatesForBuses(transport_catalogue);
    renderer::MapRenderer map(q.GetRendererSettings(), coordinates_array);

    

    
    
    //transport::reader::TextReader text_reader(std::cin);
    //transport::statistics::StatisticsJsonOutput statistics_json_output(std::cout);

    // заполнение
    //transport::RequestsProcess(
    //    &json_reader,
    //    0,
    //    transport_catalogue,
    //    &statistics_json_output
    //);

    /*
    RenderSettings rs = ExtractRenderSettings( json_reader.GetRawRenderSettings() );
    std::vector<geo::Coordinates> stops_coordinates = transport::statistics::GetStopsCoordinatesForBuses(transport_catalogue);
    MapRenderer map(rs, stops_coordinates);
    
    std::set<std::string> unique_stop_names_;
    for (std::string_view busname : transport_catalogue.GetBusesList()){
        const domain::Bus& next_bus = transport_catalogue.FindBus(std::string(busname));
        if (next_bus == domain::invalid_bus) {
            continue;
        }
        auto stops_for_bus = transport_catalogue.GetUniqueStopNamesForBus(next_bus);
        unique_stop_names_.insert(stops_for_bus.begin(), stops_for_bus.end() );
        map.AddBus(next_bus);
    }    
    for (const std::string& stop_name : unique_stop_names_) {
        const domain::Stop& next_stop = transport_catalogue.FindStop(stop_name);
        if (next_stop != domain::invalid_stop) {
            map.AddStop(next_stop);
        }        
    }
    map.Render(std::cout);*/

	return 0;
}


//RenderSettings ExtractRenderSettings(const json::Dict& source) {
//    RenderSettings render_settings;
//    try {
//        render_settings.width = source.at("width").AsDouble();
//        render_settings.height = source.at("height").AsDouble();
//        render_settings.padding = source.at("padding").AsDouble();
//        render_settings.line_width = source.at("line_width").AsDouble();
//        render_settings.stop_radius = source.at("stop_radius").AsDouble();
//        render_settings.bus_label_font_size = source.at("bus_label_font_size").AsInt();
//        render_settings.bus_label_offset.first = source.at("bus_label_offset").AsArray().at(0).AsDouble();
//        render_settings.bus_label_offset.second = source.at("bus_label_offset").AsArray().at(1).AsDouble();
//        render_settings.stop_label_font_size = source.at("stop_label_font_size").AsInt();
//        render_settings.stop_label_offset.first = source.at("stop_label_offset").AsArray().at(0).AsDouble();
//        render_settings.stop_label_offset.second = source.at("stop_label_offset").AsArray().at(1).AsDouble();  
//        render_settings.underlayer_color = GetSvgColor(source.at("underlayer_color"));
//        render_settings.underlayer_width = source.at("underlayer_width").AsDouble();
//        for (auto next_color : source.at("color_palette").AsArray()) {
//            render_settings.color_palette.push_back( GetSvgColor(next_color) );
//        }
//        //q.render_settings.color_palette
//    }
//    catch (...)
//    {
//        std::cout << "Wrong render settings format"s;
//    }
//    return render_settings;
//}

