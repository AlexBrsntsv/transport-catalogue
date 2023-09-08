//#pragma once

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/

#pragma once

#include <string>
#include "transport_catalogue.h"
#include <queue>
#include <iostream>
#include "json.h"


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

	virtual void Add(const transport::catalogue::BusInfo& bus_info, int id) = 0;
	virtual void Add(const std::string& stop_name, std::vector<std::string_view>, int id) = 0;
	virtual void AddError(std::string stop_or_bus_name, RequestError error_mark, int id) = 0;
	virtual void Show() = 0;
	virtual bool Ready() = 0;

protected:
	std::ostream& output_;	
};



class StatisticsTextOutput final : public StatisticsBaseOutput {
public:
	explicit StatisticsTextOutput(std::ostream& output) : StatisticsBaseOutput(output) {}
	void Add(const transport::catalogue::BusInfo& bus_info, int id = -1) override;
	void Add(const std::string& stop_name, std::vector<std::string_view>, int id = -1) override;	
	void AddError(std::string name, RequestError error_mark, int id = -1) override;
	void Show() override;
	bool Ready() override;
private:
	std::queue<std::string> buffer_;
};


class StatisticsJsonOutput final : public StatisticsBaseOutput {
public:
	explicit StatisticsJsonOutput(std::ostream& output) : StatisticsBaseOutput(output), buffer_(json::Array{}) {}
	void Add(const transport::catalogue::BusInfo& bus_info, int id) override;
	void Add(const std::string& stop_name, std::vector<std::string_view>, int id) override;
	void AddError(std::string name, RequestError error_mark, int id) override;
	void Show() override;
	bool Ready() override;
private:
	json::Array buffer_;
};


} // end of namespace statistics

} // end of namespace transport




