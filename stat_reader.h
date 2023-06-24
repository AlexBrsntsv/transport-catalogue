#pragma once

#include <string>
#include "transport_catalogue.h"
#include "input_reader.h"



namespace transport {

namespace reader {

namespace statistics {

void Proccess(std::istream& in,  int query_num, const transport::catalogue::TransportCatalogue& tc, std::ostream& out);

}

} // end of namespace reader

} // end of namespace transport

