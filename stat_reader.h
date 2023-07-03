#pragma once

#include <string>
#include "transport_catalogue.h"
#include "input_reader.h"



namespace transport {

namespace reader {

namespace statistics {

std::string ShowInfo(const transport::catalogue::TransportCatalogue& tc, const  Query& q);

}

} // end of namespace reader

} // end of namespace transport

