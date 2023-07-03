#pragma once
#include "transport_catalogue.h"
#include <sstream>
#include <queue>
#include "domain.h"


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
	QueryType type;
	std::string stop_name_info;
	std::string bus_name_info;
	std::pair <std::string, std::vector<std::string>> busname_to_route;
	domain::Stop stop;
	std::vector<std::pair<std::string, int>> stop_distancies;
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

	void ProcessQuery(Query&& q){
		if ((q.type == QueryType::AddStop) && !q.stop_distancies.empty()) {
			Query extra_q;
			extra_q.type = QueryType::Distancies;
			extra_q.stop = q.stop;
			extra_q.stop_distancies.swap(q.stop_distancies);
			queue_.Add(extra_q, GetPriority(extra_q.type));
		}
		queue_.Add(q, GetPriority(q.type));
	}

	Query GetNext() {
		return (queue_.GetNext());
	}
	bool Empty() {
		return (queue_.Empty());
	}

private:
	//virtual Query ExtractQuery(std::istream& input_) = 0; // extracts query from source stream
	InputPriorityQueue<Query> queue_{ static_cast<size_t>( QueryType::QueryTypesAmount ) };

protected:
	std::istream& input_;
};


class TextReader final : public InputReader {
public:
	TextReader(std::istream& in) : InputReader(in) {}
	void Process(size_t query_num) override {
		for (size_t n = 0; n < query_num; ++n) {
			ProcessQuery(ExtractQuery(input_));
		}
	}
private:
	Query ExtractQuery(std::istream& input_);
};


void Process(
	InputReader* reader,
	int query_num,
	transport::catalogue::TransportCatalogue& db,
	std::ostream& out);



} // end of namespace reader


} // end of namespace transport