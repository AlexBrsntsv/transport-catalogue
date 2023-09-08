#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;    
    using Array = std::vector<Node>;


    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

   
    class Node{
        using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;
        friend bool operator==(const Node& lhs, const Node& rhs);
    public:
       
                
        /* Реализуйте Node, используя std::variant */

        Node() = default;
        Node(Array array) :value_(move(array)) {};
        Node(Dict map) :value_(move(map)) {};
        Node(int value) :value_(value) {};
        Node(std::string value) : value_(move(value)) {};
        Node(double value) :value_(value) {};
        Node(bool value) :value_(value) {};
        Node(nullptr_t) :value_(nullptr) {};

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        bool IsInt() const;
        bool IsPureDouble() const;
        bool IsDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

    private:
        Value value_;

    };

    std::ostream& operator<<(std::ostream&, const Node& node);

    bool operator==(const Node& lhs, const Node& rhs);
    bool operator!=(const Node& lhs, const Node& rhs);

    class Document {
        friend bool operator==(const Document& lhs, const Document& rhs);
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };
    bool operator==(const Document& lhs, const Document& rhs);
    bool operator!=(const Document& lhs, const Document& rhs);

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json