#include <iostream>
#include "tests.h"
#include <sstream>
#include <iomanip>
#include "input_reader.h"
#include "json_reader.h"

#include "transport_catalogue.h"
#include "stat_reader.h"
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
	
	
	//transport::catalogue::TransportCatalogue transport_catalogue;
	//transport::reader::TextReader text_reader(std::cin);
	//transport::reader::Process(&text_reader, ReadNumber(std::cin), transport_catalogue, std::cout); // ����������
	//transport::reader::Process(&text_reader, ReadNumber(std::cin), transport_catalogue, std::cout); // ������
        
    std::setlocale(LC_ALL, "Rus");
    std::setlocale(LC_NUMERIC, "en_US.UTF-8"); // ����� std::stod  �������� ���������

    std::istringstream ss(
        " {"s
        "          \"base_requests\": ["s
        "{"s
        "  \"type\": \"Bus\","s
        "  \"name\" : \"114\","s
        " \"stops\" : [\"������� ������\", \"���������� ����\"] ,"s
        "  \"is_roundtrip\" : false"s
        " },"s
        " {"s
        "\"type\": \"Stop\","s
        "\"name\" : \"���������� ����\","s
        " \"latitude\" : 43.587795,"s
        " \"longitude\" : 39.716901,"s
        "\"road_distances\" : {\"������� ������\": 850}"
        "},"s
        "{"s
        " \"type\": \"Stop\","s
        "\"name\" : \"������� ������\","
        "\"latitude\" : 43.581969,"
        "\"longitude\" : 39.719848,"
        "\"road_distances\" : {\"���������� ����\": 850}"s
        "}"s
        "],"s
        "\"stat_requests\": ["s
        " { \"id\": 1, \"type\" : \"Stop\", \"name\" : \"���������� ����\" },"s
        " { \"id\": 2, \"type\" : \"Bus\", \"name\" : \"114\" }"s
        "]"s
        "}"s
    );

    transport::catalogue::TransportCatalogue transport_catalogue;
    transport::reader::JsonReader json_reader(ss);
    transport::reader::Process(&json_reader, -1, transport_catalogue, std::cout); // ����������-������
   








	return 0;
}