#pragma once

#include <string>
#include "transport_catalogue.h"
#include "input_reader.h"



namespace transport {

namespace output {

std::string ShowInfo(const transport::catalogue::TransportCatalogue& tc, const transport::input::Query& q);

} // end of namespace transport

} // end of namespace output