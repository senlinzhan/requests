#ifndef RESPONSE_H
#define RESPONSE_H

#include "Exception.hpp"
#include "Utils.hpp"

#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace requests {

class Response
{
public:
    using String     = std::string;
    using StringList = std::vector<String>;
    using StringMap  = std::unordered_map<String, String>;
    
    Response(const String &headers, const String &content)
        : headersStr_(headers),
          content_(content)
    {
        auto lines = splitString(headersStr_, "\r\n");

        auto &statusLine = lines[0];
        splitStatusLine(statusLine);
                
        for (int i = 1; i < lines.size(); ++i)
        {
            auto &header = lines[i];
            splitHeader(header);
        }
    }

    unsigned short statusCode() const
    {
        return statusCode_;
    }

    const String &content() const
    {
        return content_;
    }

    const String &statusMessage() const
    {
        return statusMessage_;
    }

    const StringMap &headers() const
    {
        return headers_;
    }   

    StringMap &headers()
    {
        return headers_;
    }
    
private:
    void splitStatusLine(const String &statusLine)
    {
        auto tokens = splitString(statusLine, " ");
        
        bool invalidFormat = false;
        
        if (tokens.size() != 3)
        {
            invalidFormat = true;
        }
        else
        {
            try {
                std::stoi(tokens[1]);
            }
            catch (const std::invalid_argument &e)
            {
                invalidFormat = true;
            }            
        }

        if (invalidFormat)
        {
            throw Exception("Status message in server response headers is invalid");            
        }

        version_ = std::move(tokens[0]);
        statusCode_ = std::stoi(tokens[1]);
        statusMessage_ = std::move(tokens[2]);
    }

    void splitHeader(const String &header)
    {
        auto tokens = splitString(header, ": ");

        if (tokens.size() != 2)
        {
            throw Exception("Server response contains invalid header");
        }

        headers_[std::move(tokens[0])] = std::move(tokens[1]);
    }
    
    String                               headersStr_;
    String                               content_;
    String                               version_;
    String                               statusMessage_;
    unsigned short                       statusCode_;        
    StringMap                            headers_;
};

} // namespace requests

#endif /* RESPONSE_H */
