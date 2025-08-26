//[INCLUDES]
#include "../../inc/utils/string_utils.h"

#include <algorithm>
#include <cctype>
#include <sstream>

//[NAMESPACE]
namespace StringUtils
{
    std::string Replace(const std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty()) return str;

        std::string result = str;
        size_t start_pos = 0;

        while ((start_pos = result.find(from, start_pos)) != std::string::npos)
        {
            result.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }

        return result;
    }

    bool Contains(const std::string& str, const std::string& substr)
    {
        return str.find(substr) != std::string::npos;
    }

    bool StartsWith(const std::string& str, const std::string& prefix)
    {
        return str.size() >= prefix.size() &&
               std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool EndsWith(const std::string& str, const std::string& suffix)
    {
        return str.size() >= suffix.size() &&
               std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    std::string ToLower(const std::string& str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    std::string ToUpper(const std::string& str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    std::string Trim(const std::string& str)
    {
        return TrimLeft(TrimRight(str));
    }

    std::string TrimLeft(const std::string& str)
    {
        size_t start = str.find_first_not_of(" \t\n\r\f\v");
        return (start == std::string::npos) ? "" : str.substr(start);
    }

    std::string TrimRight(const std::string& str)
    {
        size_t end = str.find_last_not_of(" \t\n\r\f\v");
        return (end == std::string::npos) ? "" : str.substr(0, end + 1);
    }

    std::vector<std::string> Split(const std::string& str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, delimiter))
        {
            tokens.push_back(item);
        }

        return tokens;
    }

    std::string Join(const std::vector<std::string>& elements, const std::string& separator)
    {
        if (elements.empty()) return "";

        std::ostringstream oss;
        auto it = elements.begin();
        oss << *it++;

        for (; it != elements.end(); ++it)
        {
            oss << separator << *it;
        }

        return oss.str();
    }

    bool IsNumeric(const std::string& str)
    {
        if (str.empty()) return false;

        bool decimal_found = false;
        size_t i = 0;

        if (str[0] == '-' || str[0] == '+') i = 1;

        for (; i < str.size(); ++i)
        {
            if (str[i] == '.')
            {
                if (decimal_found) return false;
                decimal_found = true;
            }
            else if (!std::isdigit(static_cast<unsigned char>(str[i])))
            {
                return false;
            }
        }

        return true;
    }
}
