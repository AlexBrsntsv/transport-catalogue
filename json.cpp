#include "json.h"
#include <optional>
#include <sstream>
#include <optional>
#include <cmath>

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        if (c == ' ') continue;
        result.push_back(LoadNode(input));
    }
    if (!input)  throw json::ParsingError("Incorrect json document");
    return Node(move(result));
}
/*
int LoadInt(istream& input) {
    int result = 0;
    while (isdigit(input.peek())) {
        result *= 10;
        result += input.get() - '0';
    }
    return result;
}

double LoadFract(istream& input) {
    double result = 0.0;
    double k = 1.0;
    while (isdigit(input.peek())) {
        k /= 10;
        result += (input.get() - '0') * k;
    }
    return result;
}

int LoadExp(istream& input) {
    // is number negative?
    char next_symbol = input.peek();
    bool minus_sign_mark = next_symbol == '-';
    bool plus_sign_mark = next_symbol == '+';
    if (minus_sign_mark || plus_sign_mark)  input.get();

    return LoadInt(input) * (minus_sign_mark ? -1 : 1);
}
*/
Node LoadNumber(istream& input) {
    std::string s;
    s.reserve(10);
    bool double_number_mark = false;
    bool mark = false;
    char next_char = input.peek();
    while (
        input && (
        (std::isdigit(next_char)) ||
        (next_char == '-') ||
        (next_char == '+') ||
        (mark = (next_char == '.') || (next_char == 'e') || (next_char == 'E'))
        ) )
    {
        if (mark) double_number_mark = true;
        s.push_back(input.get());
        next_char = input.peek();
    }
    try {
        if (double_number_mark) return std::stod(s);
        else return std::stoi(s);
    }
    catch (...) {
        throw json::ParsingError("Incorrect json document");
    }
}

std::optional<Node> LoadBool(istream& input) {
    std::string res;
    for (size_t i = 0; i < "true"s.size(); ++i) {
        res += input.get();
    }
    if (res == "true")   return Node(true);
    res += input.get();
    if (res == "false")  return Node(false);

    while (!res.empty()) {
        input.putback(res.back());
        res.pop_back();
    }
    return std::nullopt;
}

std::optional<Node> LoadNull(istream& input) {
    std::string res;
    for (size_t i = 0; i < "null"s.size(); ++i) {
        res += input.get();
    }
    if (res == "null") return Node(nullptr);

    while (!res.empty()) {
        input.putback(res.back());
        res.pop_back();
    }
    return std::nullopt;
}


std::pair<char, bool> GetNextChar(istream& input) {
   
    char curr_c = input.get();   
    char next_c = input.peek();
    
    char result = curr_c;
    if (curr_c == '\\') {
        switch (next_c) {
        case 'n':            
            result = '\n';
            input.get();
            next_c = input.peek();
            break;
        case 'r':
            result = '\r';
            input.get();
            next_c = input.peek();
            break;
        case '"':
            result = '"';
            input.get();
            next_c = input.peek();
            break;
        case 't':
            result = '\t';
            input.get();
            next_c = input.peek();
            break;
        case '\\':
            result = '\\';
            input.get();
            next_c = input.peek();
            break;
        default:
            break;
        }
    }  
    bool end_of_string_mark = false;
    if (next_c == '\"') {
        end_of_string_mark = true;
        input.get();
    }

    return { result, end_of_string_mark };
}

    /*
std::optional<char> GetEscapeChar(char c1, char c2) {
    char result;
    if (c1 == '\\') {
        switch (c2) {
        case 'n':
            result = '\n';
            break;
        case 'r':
            result = '\r';
            break;
        case '"':
            result = '"';
            break;
        case 't':
            result = '\t';
            break;
        case '\\':
            result = '\\';
            break;
        default:
            return nullopt;
            break;
        }
    }
    else {
        return nullopt;
    }

    return result;
}

Node LoadString_(istream& input) {
    string line;
    char c;
    input >> std::noskipws;
    while (input >> c) {
        if (const auto opt_c = GetEscapeChar(c, input.peek()); opt_c) {
            line += *opt_c;
            input.get();
        }
        else {
            line += c;
        }
        if (input.peek() == '\"') {
            input.get();
            break;
        }
    }
    if (!input)  throw json::ParsingError("Incorrect json document");
    input >> std::skipws;
    return Node(move(line));
}
*/
Node LoadString(istream& input) {
    string line;    
    input >> std::noskipws;
    bool end_of_string = false;
    do {
        const auto [next_c, end_mark] = GetNextChar(input);
        line += next_c;
        end_of_string = end_mark;
    } while (input && !end_of_string);

    if (!input)  throw json::ParsingError("Incorrect json document");
    input >> std::skipws;
    return Node(move(line));
}

Node LoadDict(istream& input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({ move(key), LoadNode(input) });
    }

    if (!input)  throw json::ParsingError("Incorrect json document");

    return Node(move(result));
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    }
    else if (c == '{') {
        return LoadDict(input);
    }
    else if (c == '"') {
        return LoadString(input);
    }
    else if (std::isdigit(c) || (c == '-' && std::isdigit(input.peek()))) {
        input.putback(c);
        return LoadNumber(input);
    }
    else if (c == 't' || c == 'f') {
        input.putback(c);
        if (const auto result = LoadBool(input); result) { return *result; }
    }
    else if (c == 'n') {
        input.putback(c);
        if (const auto result = LoadNull(input); result) { return *result; }
    }
    throw json::ParsingError("Incorrect json document");
}

std::string ShowWithEscapeChars(const std::string& s) {
    std::string result;
    for (const char c : s) {
        switch (c) {
        case '\n':
            result += "\\n"s;
            break;
        case '\r':
            result += "\\r"s;
            break;
        case '\"':
            result += "\\\""s;
            break;
        case '\t':
            result += c; // \t не экранируется
            break;
        case '\\':
            result += "\\\\"s;
            break;
        default:
            result += c;
            break;
        }
    }
    return result;
}

}  // namespace




    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }
    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsDouble() const {
        return IsInt() || IsPureDouble();
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }
    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }
    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }
    
    bool operator==(const Node& lhs, const Node& rhs) {       
        return lhs.value_ == rhs.value_;
    }
    
    bool operator!=(const Node& lhs, const Node& rhs) {
        return !(rhs == lhs);
    }
    

    int Node::AsInt() const {
        if (IsInt()) return std::get<int>(value_);
        throw std::logic_error("unavailable operation");
    }
    bool Node::AsBool() const {
        if (IsBool()) return std::get<bool>(value_);
        throw std::logic_error("unavailable operation");
    }
    double Node::AsDouble() const {
        if (IsPureDouble()) return std::get<double>(value_);
        if (IsInt()) return std::get<int>(value_);
        throw std::logic_error("unavailable operation");
    }
    const std::string& Node::AsString() const {
        if (IsString()) return std::get<std::string>(value_);
        throw std::logic_error("unavailable operation");
    }
    const Array& Node::AsArray() const {
        if (IsArray()) return std::get<Array>(value_);
        throw std::logic_error("unavailable operation");
    }
    const Dict& Node::AsMap() const {
        if (IsMap()) return std::get<Dict>(value_);
        throw std::logic_error("unavailable operation");
    }


    Document::Document(Node root)
        : root_(move(root)) {
    }

    bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.root_ == rhs.root_;
    }
    bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    std::ostream& operator<<(std::ostream& out, const Node& node) {
        if (node.IsNull()) out << "null"sv;
        else if (node.IsInt()) out << node.AsInt();
        else if (node.IsDouble()) out << node.AsDouble();
        else if (node.IsString()) out << '\"' << ShowWithEscapeChars(node.AsString()) << '\"';
        else if (node.IsBool()) out << boolalpha << node.AsBool();
        else if (node.IsMap()) {
            out << '{';
            bool first_time = true;
            for (const auto& [key, value] : node.AsMap()) {
                if (first_time) first_time = false;
                else out << ","s;
                out << Node(key) << ":"s << Node(value);
            }
            out << '}';
        }
        else if (node.IsArray()) {
            out << '[';
            bool first_time = true;
            for (const auto& value : node.AsArray()) {
                if (first_time) first_time = false;
                else out << ","s;

                out << Node(value);
            }
            out << ']';
        }
        else {
            out << "output error"s << std::endl;
        }
        return out;
    }

    void Print(const Document& doc, std::ostream& output) {
        output << doc.GetRoot();
    }


}  // namespace json