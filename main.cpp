#include <iostream>
//#include "tests.h"
#include <sstream>
#include <iomanip>
//#include "input_reader.h"
#include "json_reader.h"

#include "transport_catalogue.h"
#include <sstream>
#include "json.h"
using namespace std;
using namespace transport;

int ReadNumber(std::istream& is) {		
	std::string s;
	std::getline(is, s);
	return std::stoi(s);
}

int main() {	
	//transport::tests::Catalogue();
	//transport::tests::Input();  

	
	
  
    std::setlocale(LC_ALL, "Rus");
    std::setlocale(LC_NUMERIC, "en_US.UTF-8"); // чтобы std::stod работала корректно
    
    //std::istringstream ss(
    //    " {"s
    //    "          \"base_requests\": ["s
    //    "{"s
    //    "  \"type\": \"Bus\","s
    //    "  \"name\" : \"114\","s
    //    " \"stops\" : [\"Морской вокзал\", \"Ривьерский мост\"] ,"s
    //    "  \"is_roundtrip\" : false"s
    //    " },"s
    //    " {"s
    //    "\"type\": \"Stop\","s
    //    "\"name\" : \"Ривьерский мост\","s
    //    " \"latitude\" : 43.587795,"s
    //    " \"longitude\" : 39.716901,"s
    //    "\"road_distances\" : {\"Морской вокзал\": 850}"
    //    "},"s
    //    "{"s
    //    " \"type\": \"Stop\","s
    //    "\"name\" : \"Морской вокзал\","
    //    "\"latitude\" : 43.581969,"
    //    "\"longitude\" : 39.719848,"
    //    "\"road_distances\" : {\"Ривьерский мост\": 850}"s
    //    "}"s
    //    "],"s
    //    "\"stat_requests\": ["s
    //    " { \"id\": 1, \"type\" : \"Stop\", \"name\" : \"Ривьерский мост\" },"s
    //    " { \"id\": 2, \"type\" : \"Bus\", \"name\" : \"114\" }"s
    //    "]"s
    //    "}"s

    //);  
    std::istringstream ss(
        
        " {"s
        "          \"render_settings\": "s
        "{"s
        "\"width\": 1200.0,"s
        " \"height\" : 1200.0,"s
        " \"padding\" : 50.0,"s
        " \"line_width\" : 14.0,"s
        "  \"stop_radius\" : 5.0,"s

        " \"bus_label_font_size\" : 20,"s
        "\"bus_label_offset\" : [7.0, 15.0] ,"s

        " \"stop_label_font_size\" : 20,"s
        "\"stop_label_offset\" : [7.0, -3.0] ,"s

        "\"underlayer_color\" : [255, 255, 255, 0.85] ,"s
        "\"underlayer_width\" : 3.0,"s

        " \"color_palette\" : ["s
        " \"green\","s
        " [255, 160, 0],"s
        " \"red\""s
        " ]"
        "}"s
        " }"s
        

    );
    
    

    transport::catalogue::TransportCatalogue transport_catalogue;    
    //transport::reader::JsonReader json_reader(std::cin);
    transport::reader::JsonReader json_reader(ss);
    //transport::reader::TextReader text_reader(std::cin);
    transport::statistics::StatisticsJsonOutput statistics_json_output(std::cout);

    // заполнение
    transport::RequestsProcess(
        &json_reader,
        0,
        transport_catalogue,
        &statistics_json_output
    );

	return 0;
}