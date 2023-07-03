#pragma once
#include "input_reader.h"
#include "json.h"

namespace transport {

namespace reader{

class JsonReader final : public transport::reader::InputReader {
public:
	JsonReader(std::istream& input) : InputReader(input) { }

	void Process(size_t) override;

private:
	Query ExtractQuery(const json::Dict& source);
};


} // end of namespace reader

} // end of namespace transport