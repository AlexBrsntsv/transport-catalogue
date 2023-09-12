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


void MapRenderer::AddRouteView(std::vector<geo::Coordinates>&& geo_coords) {
    if (geo_coords.empty()) return;
    
    auto route_view = std::make_unique<svg::Polyline>();
    for (const auto& node_coordinates : geo_coords) {
        route_view->AddPoint( sphere_projector_(node_coordinates) );
    }
    route_view->SetFillColor(svg::NoneColor);
    route_view->SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    route_view->SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route_view->SetStrokeColor( render_settings_.color_palette.at(color_idx_) );
    route_view->SetStrokeWidth( render_settings_.line_width );
    if ( ++color_idx_ >= render_settings_.color_palette.size() ) {
        color_idx_ = 0;
    }
    AddPtr( std::move(route_view) );
}


