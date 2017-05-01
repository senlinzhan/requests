#ifndef URL_H
#define URL_H

#include "Utils.hpp"
#include "Exception.hpp"
#include <string>

class Url
{
public:
    explicit Url(const std::string &url)
        : url_(url)
    {
        auto tokens = splitTokens(url, "://");        
        if (tokens.size() != 2)
        {
            throw Exception("Invalid URL - no schema supplied: " + url_);                        
        }

        schema_ = std::move(tokens[0]);        
        auto url_no_schema = std::move(tokens[1]);
        
        auto pos1 = url_no_schema.find("/");        
        if (pos1 == std::string::npos)
        {
            host_ = std::move(url_no_schema);
            path_ = "/";
        }
        else
        {
            host_ = url_no_schema.substr(0, pos1);
            path_ = url_no_schema.substr(pos1);
        }
    }

    const std::string &host() const
    {
        return host_;
    }

    const std::string &schema() const
    {
        return schema_;
    }

    const std::string &path() const
    {
        return path_;
    }
    
private:
    std::string url_;
    std::string schema_;
    std::string host_;
    std::string path_;    
};

#endif /* URL_H */
