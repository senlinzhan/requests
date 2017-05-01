#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <iostream>

std::vector<std::string> splitTokens(const std::string &str, const std::string &separator)
{
    std::vector<std::string> results;

    std::size_t beginPos = 0;
    std::size_t pos = str.find(separator, beginPos);
    
    while (pos != std::string::npos)
    {
        if (pos > beginPos)
        {
            results.push_back(str.substr(beginPos, pos - beginPos));            
        }

        beginPos = pos + separator.size();

        if (beginPos >= str.size())
        {
            break;
        }
        pos = str.find(separator, beginPos);
    }    

    if (beginPos < str.size())
    {
        results.push_back(str.substr(beginPos));
    }
    
    return results;
}


#endif /* UTILS_H */
