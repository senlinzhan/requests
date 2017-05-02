#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <iostream>

namespace requests {

std::vector<std::string> splitString(const std::string &str, const std::string &separator)
{
    std::vector<std::string> results;

    std::size_t first = 0, last = 0;

    while (first < str.size())
    {
        last = str.find(separator, first + 1);
        if (last == std::string::npos)
        {
            results.push_back(str.substr(first));
            break;
        }
        results.push_back(str.substr(first, last - first));
        first = last + separator.size();
    }

    return results;    
}

std::vector<std::string> splitString(const std::string &str, const std::string &separator, std::size_t splitTimes)
{
    if (splitTimes == 0)
    {
        return { str };
    }

    std::size_t first = 0, last = 0, times = 0;
    std::vector<std::string> results;    

    while (first < str.size())
    {
        last = str.find(separator, first + 1);
        if (last == std::string::npos)
        {
            results.push_back(str.substr(first));
            break;
        }
        results.push_back(str.substr(first, last - first));
        first = last + separator.size();

        if (++times == splitTimes)
        {
            results.push_back(str.substr(first));
            break;
        }
    }

    return results;    
}

} // namespace requests

#endif /* UTILS_H */
