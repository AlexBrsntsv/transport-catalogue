#pragma once
#include "transport_catalogue.h"
#include <sstream>
#include <queue>
#include <memory.h>
#include "domain.h"
#include "request_handler.h"
#include "map_renderer.h"



using namespace std::literals;

namespace transport {

namespace reader {

class InputReader {
public:
	InputReader(std::istream& input): input_(input){ }	
	virtual transport::requests::QueriesQueue Process() = 0;	
protected:
	virtual ~InputReader() = default;
	std::istream& input_;		
};    

//---------------- json ---------------------
class JsonReader final : public transport::reader::InputReader {
public:
	JsonReader(std::istream& input) : InputReader(input) { }
	transport::requests::QueriesQueue Process() override;
private:
	transport::requests::Query ExtractQuery(const json::Dict& source);
	renderer::RenderSettings ExtractRenderSetts(const json::Dict& source) const;
	svg::Color GetSvgColor(const json::Node& source) const;
};


} // end of namespace reader


} // end of namespace transport


