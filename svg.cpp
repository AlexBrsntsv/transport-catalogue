#include "svg.h"

namespace svg {

using namespace std::literals;

std::ostream& operator<< (std::ostream& os, const StrokeLineCap& line_cap) {

    switch (line_cap) {
    case StrokeLineCap::BUTT:
        os << "butt";
        break;
    case StrokeLineCap::ROUND:
        os << "round";
        break;
    case StrokeLineCap::SQUARE:
        os << "square";
        break;
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const StrokeLineJoin& line_join) {
  
    switch (line_join) {
    case StrokeLineJoin::ARCS:
        os << "arcs";
        break;
    case StrokeLineJoin::BEVEL:
        os << "bevel";
        break;
    case StrokeLineJoin::MITER:
        os << "miter";
        break;
    case StrokeLineJoin::MITER_CLIP:
        os << "miter-clip";
        break;
    case StrokeLineJoin::ROUND:
        os << "round";
        break;
    }
    return os;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(out);
    out << "/>"sv;
}


// ---------- Polyline ------------------
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool first_time = true;
    for (const auto& [x, y] : points_) {
        if (first_time) first_time = false;
        else out << ' ';

        out << x << ","sv << y;
    }    
    out << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ---------- Text ------------------
// Задаёт координаты опорной точки (атрибуты x и y)
Text& Text::SetPosition(Point pos) { 
    position_ = pos;  
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) { 
    offset_ = offset; 
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) { 
    font_size_ = size; 
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(const std::string& font_family) { 
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(const std::string& font_weight) { 
    font_weight_ = font_weight; 
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(const std::string& data) { 
    data_ = SvgTextAdaptor(data);
    return *this;
}

std::string Text::SvgTextAdaptor(const std::string& source) {
    std::string result = ""s;
    for (const char c : source) {
        
        switch (c) {
            case '"':
                result += "&quot;";
                break;
            case '\'':
                result += "&apos;";
                break;
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '&':
                result += "&amp;";
                break;
            default:
                result += c;
                break;
            
        }
    }

    return result;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text"sv;
    RenderAttrs(out);
    out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\" "sv;
    out << "dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" "sv;
    out << "font-size=\""sv << font_size_ << "\""sv;
    if (!font_family_.empty()) { out << " font-family=\""sv << font_family_ << "\""sv; }
    if (!font_weight_.empty()) { out << " font-weight=\""sv << font_weight_ << "\""sv; }
    
    out << ">"sv << data_;
    out << "</text>"sv;
}


// Добавляет в svg-документ объект-наследник svg::Object
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.push_back( std::move(obj) );
}

// Выводит в ostream svg-представление документа
void Document::Render(std::ostream& out) const {
    out << head_ << std::endl;

    for (const auto& obj: objects_) {
        obj->Render( RenderContext(out, 0, 2) );
    }
    out << tail_;
}







}  // namespace svg