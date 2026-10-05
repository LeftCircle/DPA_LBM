#pragma once



#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <sstream>
#include <any>
#include <functional>
#include <map>

// inline std::string json_string(const std::string& s) {
//     std::string out = "\"";
//     for (unsigned char c : s) {
//         switch (c) {
//         case '"':  out += "\\\""; break;
//         case '\\': out += "\\\\"; break;
//         case '\b': out += "\\b";  break;
//         case '\f': out += "\\f";  break;
//         case '\n': out += "\\n";  break;
//         case '\r': out += "\\r";  break;
//         case '\t': out += "\\t";  break;
//         default:propogate_to_neighbors
//             if (c < 0x20) {
//                 const char hex[] = "0123456789abcdef";
//                 out += "\\u00";
//                 out += hex[c >> 4];
//                 out += hex[c & 0x0f];
//             } else {
//                 out += static_cast<char>(c);
//             }
//         }
//     }
//     return out + '"';
// }

// inline std::string encode_json(const std::string& s) {
//     return json_string(s);
// }

// inline std::string encode_json(const char* s) {
//     return json_string(s);
// }

// std::string encode_json(const pba::Vector& v) {
//     return "[" + encode_json(v.x()) + "," +
//                  encode_json(v.y()) + "," +
//                  encode_json(v.z()) + "]";
// }

// template <class T>
// std::string encode_json(T value) {
//     static_assert(std::is_arithmetic_v<T>,
//                   "Add an encode_json overload for this type");

//     if constexpr (std::is_floating_point_v<T>) {
//         if (!std::isfinite(value)) {
//             throw std::invalid_argument("JSON cannot represent NaN or infinity");
//         }
//     }

//     std::ostringstream out;
//     out << std::setprecision(std::numeric_limits<T>::max_digits10) << value;
//     return out.str();
// }

// template <typename T>
// using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;

// using ValueWriter = std::function<std::string(const std::any&)>;


// class DataLogger {
// public:
//     explicit DataLogger(std::string filename)
//         : _filename(std::move(filename)) {}

//     template <class... Keys>
//     void set_keys(Keys&&... keys) {
//         _current_keys.clear();
//         (_current_keys.emplace_back(std::forward<keys>), ...);
//         for (const auto& key : _current_keys) {
//             _add_key(key);
//         }
//     }


//     template <class... Args>
//     void log_keyed(Args&&... args) {
//         std::vector<std::string> encoded{
//             encode_json(std::forward<Args>(args))...
//         };

//         if (encoded.size() > _keys.size()) {
//             throw std::invalid_argument("More values than configured keys");
//         }

//         for (std::size_t i = 0; i < encoded.size(); ++i) {
//             _values[i].push_back(std::move(encoded[i]));
//         }
//     }

//     void save_to_file() const {
//         std::ofstream out(_filename);
//         if (!out) {
//             throw std::runtime_error("Could not open output file");
//         }

//         out << "{\n";
//         for (std::size_t i = 0; i < _keys.size(); ++i) {
//             out << "  " << _keys[i] << ": [";

//             for (std::size_t j = 0; j < _values[i].size(); ++j) {
//                 if (j != 0) out << ", ";
//                 out << _values[i][j];
//             }

//             out << "]";
//             if (i + 1 != _keys.size()) out << ",";
//             out << "\n";
//         }
//         out << "}\n";
//     }

// private:
//     template <class Key>
//     void _add_key(const Key& key) {
//         std::string name(std::forward<Key>(key));
//         _values.try_emplace(name, std::vector<std::string>{});
//     }
// private:
//     std::string _filename;
//     std::vector<std::string> _current_keys;
//     std::map<std::string, std::vector<std::string>> _values;
//     std::map<std::string, ValueWriter> _writers;
// };











