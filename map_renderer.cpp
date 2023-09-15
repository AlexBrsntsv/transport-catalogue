#include "geo.h"
#include "svg.h"
#include "map_renderer.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <memory>
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

namespace renderer {

 // SphereProjector
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

// Проецирует широту и долготу в координаты внутри SVG-изображения
svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
    return {
        (coords.lng - min_lon_) * zoom_coeff_ + padding_,
        (max_lat_ - coords.lat) * zoom_coeff_ + padding_
    };
}

MapRenderer::MapRenderer(RenderSettings&& rs, const transport::catalogue::TransportCatalogue& db): render_settings_(std::move(rs)) {





}



void MapRenderer::AddBus(const domain::Bus& bus) {
    if ( bus.route.empty() ) return;

    auto route_view = std::make_unique<svg::Polyline>();
    for (const domain::Stop* stop : bus.route) {
        route_view->AddPoint(sphere_projector_(stop->coordinates));
    }
    route_view->SetFillColor(svg::NoneColor);
    route_view->SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    route_view->SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route_view->SetStrokeColor(render_settings_.color_palette.at(color_idx_));
    route_view->SetStrokeWidth(render_settings_.line_width);    
    bus_routes_.push_back(std::move(route_view));

    // подложка
    auto route_name_base_view = std::make_unique<svg::Text>();    
    
    route_name_base_view->SetOffset(
        { 
            render_settings_.bus_label_offset.first, 
            render_settings_.bus_label_offset.second 
        }
    );
    route_name_base_view->SetFontSize(render_settings_.bus_label_font_size);
    route_name_base_view->SetFontFamily("Verdana");
    route_name_base_view->SetFontWeight("bold");
    route_name_base_view->SetData(bus.name);
    route_name_base_view->SetPosition(sphere_projector_(bus.route.at(0)->coordinates));

    // 
    auto route_name_view = std::make_unique<svg::Text>(*route_name_base_view);

    // дополнительные свойства подложки   
    route_name_base_view->SetFillColor(render_settings_.underlayer_color);
    route_name_base_view->SetStrokeColor(render_settings_.underlayer_color);
    route_name_base_view->SetStrokeWidth(render_settings_.underlayer_width);
    route_name_base_view->SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route_name_base_view->SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    // дополнительное свойство самой надписи
    route_name_view->SetFillColor(render_settings_.color_palette.at(color_idx_));

    // маршрут не кольцевой
    if (!bus.is_roundtrip) {
        auto ending_stop = bus.route.begin() + bus.route.size() / 2;
        
        
        auto route_end_name_base_view = std::make_unique<svg::Text>(*route_name_base_view);
        route_end_name_base_view->SetPosition(sphere_projector_( (*ending_stop)->coordinates) );

        auto route_end_name_view = std::make_unique<svg::Text>(*route_name_view);
        route_end_name_view->SetPosition(sphere_projector_( (*ending_stop)->coordinates) );

        bus_names_.push_back(std::move(route_name_base_view));
        bus_names_.push_back(std::move(route_name_view));

        if (*ending_stop != bus.route.front()) {
            bus_names_.push_back(std::move(route_end_name_base_view));
            bus_names_.push_back(std::move(route_end_name_view));
        }
    }
    else {
        bus_names_.push_back(std::move(route_name_base_view));
        bus_names_.push_back(std::move(route_name_view));
    }

    if (++color_idx_ >= render_settings_.color_palette.size()) {
        color_idx_ = 0;
    }
}

void MapRenderer::Render(std::ostream& out){
    for (std::unique_ptr<svg::Object>& element: bus_routes_) {
        Document::AddPtr( std::move(element) );
    }
    for (std::unique_ptr<svg::Object>& element : bus_names_) {
        Document::AddPtr(std::move(element));
    }
    for (std::unique_ptr<svg::Object>& element : stops_) {
        Document::AddPtr(std::move(element));
    }
    for (std::unique_ptr<svg::Object>& element : stop_names_) {
        Document::AddPtr(std::move(element));
    }    
    svg::Document::Render(out);
}



void MapRenderer::AddStop(const domain::Stop& stop) {
    auto stop_view = std::make_unique<svg::Circle>();
    stop_view->SetCenter(sphere_projector_(stop.coordinates));
    stop_view->SetRadius(render_settings_.stop_radius);
    stop_view->SetFillColor("white");
    stops_.push_back(std::move(stop_view));

    // подложка
    auto stop_name_base_view = std::make_unique<svg::Text>();

    stop_name_base_view->SetOffset(
        {
            render_settings_.stop_label_offset.first,
            render_settings_.stop_label_offset.second
        }
    );
    stop_name_base_view->SetFontSize(render_settings_.stop_label_font_size);
    stop_name_base_view->SetFontFamily("Verdana");    
    stop_name_base_view->SetData(stop.name);
    stop_name_base_view->SetPosition( sphere_projector_(stop.coordinates) );

    // 
    auto stop_name_view = std::make_unique<svg::Text>(*stop_name_base_view);

    // дополнительные свойства подложки   
    stop_name_base_view->SetFillColor(render_settings_.underlayer_color);
    stop_name_base_view->SetStrokeColor(render_settings_.underlayer_color);
    stop_name_base_view->SetStrokeWidth(render_settings_.underlayer_width);
    stop_name_base_view->SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    stop_name_base_view->SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    // дополнительное свойство самой надписи
    stop_name_view->SetFillColor("black");

    stop_names_.push_back(std::move(stop_name_base_view));
    stop_names_.push_back(std::move(stop_name_view));
}

const RenderSettings& MapRenderer::GetSettings() const {
    return render_settings_;
}



} // end of namespace renderer


