#pragma once

#include <string>
#include "transport_catalogue.h"
#include <queue>
#include <iostream>




namespace transport {

namespace statistics{

enum class RequestError {
	Bus,
	Stop,
};

class StatisticsBaseOutput {
public:
	explicit StatisticsBaseOutput(std::ostream& output) : output_(output) {}
	virtual ~StatisticsBaseOutput() = default;

	virtual void Add(const transport::catalogue::BusInfo& bus_info) = 0;
	virtual void Add(const std::string& stop_name, std::vector<std::string_view>) = 0;
	virtual void AddError(std::string stop_or_bus_name, RequestError error_mark) = 0;
	virtual void Show() = 0;
	virtual bool Ready() = 0;

protected:
	std::ostream& output_;	
};



class StatisticsTextOutput final : public StatisticsBaseOutput {
public:
	explicit StatisticsTextOutput(std::ostream& output) : StatisticsBaseOutput(output) {}
	void Add(const transport::catalogue::BusInfo& bus_info) override;
	void Add(const std::string& stop_name, std::vector<std::string_view>) override;	
	void AddError(std::string name, RequestError error_mark) override;
	void Show() override;
	bool Ready() override;
private:
	std::queue<std::string> buffer_;
};


} // end of namespace statistics

} // end of namespace transport

