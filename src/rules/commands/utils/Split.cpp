#include "../Commands.hpp"

std::vector<std::string> split(const std::string& str, char sep)
{
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type pos = str.find(sep, start);

    while (pos != std::string::npos)
    {
        result.push_back(str.substr(start, pos - start));
        start = pos + 1;
        pos = str.find(sep, start);
    }
    result.push_back(str.substr(start));
    return result;
}
