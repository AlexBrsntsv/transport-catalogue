#pragma once

#include <string>
#include "transport_catalogue.h"
#include "input_reader.h"



namespace transport {

namespace reader {

namespace statistics {

void Show(std::istream& in, std::ostream& out, int query_num, const transport::catalogue::TransportCatalogue& tc);

}

} // end of namespace reader

} // end of namespace transport

