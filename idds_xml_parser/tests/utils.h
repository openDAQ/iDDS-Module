#include <string>
#include <algorithm>

inline std::string strip_whitespace(const std::string& input) {
    std::string result = input;
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
    return result;
}

