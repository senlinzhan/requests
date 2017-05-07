#ifndef RESPONSE_H
#define RESPONSE_H

#include "Exception.hpp"
#include "Utils.hpp"

#include <assert.h>
#include <string>
#include <unordered_map>
#include <iostream>

namespace requests {

class Response
{
public:
    Response(const std::string &headers, const std::string &content)
        : headersStr_(headers),
          content_(content)
    {
        auto lines = splitString(headersStr_, "\r\n");
        auto tokens = splitString(lines[0], " ");

        assert(tokens.size() == 3);
        
        version_ = std::move(tokens[0]);
        statusCode_ = std::stoi(tokens[1]);
        statusMessage_ = std::move(tokens[2]);

        for (int i = 1; i < lines.size(); ++i)
        {
            auto header = lines[i];
            auto kv = splitString(header, ": ");
            headers_[kv[0]] = kv[1];
        }
    }

    unsigned short statusCode() const
    {
        return statusCode_;
    }

    const std::string &content() const
    {
        return content_;
    }

    const std::string &statusMessage() const
    {
        return statusMessage_;
    }

    const std::unordered_map<std::string, std::string> &headers() const
    {
        return headers_;
    }   

    std::unordered_map<std::string, std::string> &headers()
    {
        return headers_;
    }
    
private:
    std::string                                  headersStr_;
    std::string                                  content_;
    std::string                                  version_;
    std::string                                  statusMessage_;
    unsigned short                               statusCode_;        
    std::unordered_map<std::string, std::string> headers_;
};

} // namespace requests

#endif /* RESPONSE_H */
