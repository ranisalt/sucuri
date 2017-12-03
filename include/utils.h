#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace utils {

template <typename Iterable>
inline auto join(const std::string& sep, Iterable i)
{
    std::stringstream ss;
    auto it = std::begin(i);
    if (it != std::end(i)) {
        ss << *it;
        ++it;
    }
    for (; it != std::end(i); ++it) {
        ss << sep << *it;
    }
    return ss.str();
}

template <>
inline auto join(const std::string& sep, std::vector<std::string> i)
{
    std::stringstream ss;
    auto it = std::begin(i);
    if (it != std::end(i)) {
        ss << *it;
        ++it;
    }
    for (; it != std::end(i); ++it) {
        ss << sep << *it;
    }
    return ss.str();
}

}
