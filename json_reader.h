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

enum class QueryType {	
	AddBus,
	AddStop,
	Distancies,
	BusInfo,
	StopInfo,	
	QueryTypesAmount,
	Invalid
};

size_t GetPriority(QueryType q);

struct Query {
	int id = -1;
	QueryType type = QueryType::Invalid;
	std::string stop_name_info;
	std::string bus_name_info;
	std::pair <std::string, std::vector<std::string>> busname_to_route;
	domain::Stop stop;
	std::vector<std::pair<std::string, int>> stop_distancies;
	RenderSettings render_settings;
};

bool operator==(const Query& lhs, const Query& rhs);

template<typename Value>
class InputPriorityQueue {
public:
	explicit InputPriorityQueue(size_t priority_lvl_num): priority_level_number_ (priority_lvl_num) { 
		queues_.resize(priority_lvl_num); 
	}
	InputPriorityQueue() = delete;
	void Add(const Value& value, size_t priority_lvl) {
		if (priority_lvl >= priority_level_number_) {
			throw std::invalid_argument("unexistable priority level");
		}		
		queues_[priority_lvl].push(value);
	}
	void Add(Value&& value, size_t priority_lvl) {
		if (priority_lvl >= priority_level_number_) {
			throw std::invalid_argument("unexistable priority level");
		}
		queues_[priority_lvl].push( move(value) );
	}	

	Value GetNext() {
		Value result;
		for (auto& q : queues_) {
			if (!q.empty()) {
				result = q.front();
				q.pop();
				break;
			}
		}
		return result;
	}

	bool Empty() const {		
		for (const auto& q : queues_) {
			if (!q.empty()) return false;
		}	
		return true;
	}
private:
	size_t priority_level_number_;
	std::vector<std::queue<Value>> queues_;
};


class InputReader {
public:
	InputReader(std::istream& input): input_(input){ }	
	virtual void Process(size_t query_num = 1) = 0;
	virtual ~InputReader() = default;
	void ProcessQuery(Query&& q);
	Query GetNext();
	bool Empty() const;	

private:
	//virtual Query ExtractQuery(std::istream& input_) = 0; // extracts query from source stream
	InputPriorityQueue<Query> queue_{ static_cast<size_t>( QueryType::QueryTypesAmount ) };
	

protected:
	std::istream& input_;	
};    
    
//---------------- json ---------------------
class JsonReader final : public transport::reader::InputReader {
public:
	JsonReader(std::istream& input) : InputReader(input) { }
	void Process(size_t) override;
	json::Dict& GetRawRenderSettings();

private:
	Query ExtractQuery(const json::Dict& source);
	json::Dict raw_render_settings;	
};


} // end of namespace reader

void RequestsProcess(
	transport::reader::InputReader* reader,
	int query_num,
	transport::catalogue::TransportCatalogue& db,
	transport::statistics::StatisticsBaseOutput* statistics_output
);


} // end of namespace transport


