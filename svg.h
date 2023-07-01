#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <variant>

namespace svg {


struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};
std::ostream& operator<< (std::ostream& os, const StrokeLineCap& line_cap);

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};
std::ostream& operator<< (std::ostream& os, const StrokeLineJoin& line_join);

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b): red(r), green(g), blue(b) { }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};
std::ostream& operator<< (std::ostream& os, const Rgb& rgb);

struct Rgba {
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double op): red(r), green(g), blue(b), opacity(op) { }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};
std::ostream& operator<< (std::ostream& os, const Rgba& rgba);

using Color = std::variant<std::monostate, Rgb, Rgba, std::string>;
inline const Color NoneColor;
std::ostream& operator<< (std::ostream& os, const Color& color);

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }

        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        } 

        if (stroke_width_) {
            out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
        }
        if (line_cap_) {
            out << " stroke-linecap=\""sv <<  *line_cap_  << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv <<  *line_join_  << "\""sv;
        }
    }

private:

    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};


/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:    
    void Render(const RenderContext& context) const; 
    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class ObjectContainer {
public:

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    template <typename Obj>
    void Add(Obj obj) {
        auto u_ptr = std::make_unique<Obj>( std::move(obj) );
        AddPtr( std::move(u_ptr) );
    }
    // Добавляет в svg-документ объект-наследник svg::Object
    
    // Выводит в ostream svg-представление документа
    //virtual void Render(std::ostream& out) const= 0;    
    virtual ~ObjectContainer() = default;

};



class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};


/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle : public Object, public PathProps<Circle> {
public:
    Circle() = default;   
    ~Circle() = default;
    
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);    

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline : public Object, public PathProps<Polyline> {
public:
    Polyline() = default;
    ~Polyline() = default;
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;   
    std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object, public PathProps<Text> {
public:
    Text() = default;
    ~Text() = default;
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(const std::string& font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(const std::string& font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(const std::string& data);


// Прочие данные и методы, необходимые для реализации элемента <text>
private:
    void RenderObject(const RenderContext& context) const override;

    static std::string SvgTextAdaptor(const std::string& source);
    Point position_ = { 0,0 };
    Point offset_ = { 0,0 };
    uint32_t font_size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;    
};

class Document: public ObjectContainer {
public:
    Document() = default;

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

private:
    
    static inline const std::string head_ =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";

    static inline const std::string tail_ = "</svg>";

    std::vector<std::unique_ptr<Object>> objects_;   
};

}  // namespace svg