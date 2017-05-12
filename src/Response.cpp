#include "Exception.hpp"
#include "Response.hpp"
#include "Utils.hpp"

#include <iostream>

using namespace requests;

Response::Response(const String &headers, const String &content)
    : headersStr_(headers),
      content_(content)
{
    auto lines = splitString(headersStr_, "\r\n");
    
    auto &statusLine = lines[0];
    splitStatusLine(statusLine);
    
    for (std::size_t i = 1; i < lines.size(); ++i)
    {
        auto &header = lines[i];
        splitHeader(header);
    }
}

void Response::splitStatusLine(const String &statusLine)
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
        throw Exception("Status message in server response headers is invalid: " + statusLine);            
    }

    version_ = std::move(tokens[0]);
    statusCode_ = std::stoi(tokens[1]);
    statusMessage_ = std::move(tokens[2]);
}

void Response::splitHeader(const String &header)
{
    auto tokens = splitString(header, ": ");

    if (tokens.size() != 2)
    {
        throw Exception("Server response contains invalid header");
    }

    headers_[tokens[0]] = tokens[1];
}
