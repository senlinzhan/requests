#ifndef URL_H
#define URL_H

#include "Utils.hpp"
#include "Exception.hpp"
#include <string>

namespace requests {

class Url
{
public:
    friend std::ostream &operator<<(std::ostream &os, const Url &url);
    
    explicit Url(const std::string &url)
        : url_(url)
    {
        auto tokens = splitString(url, "://");        
        if (tokens.size() != 2)
        {
            throw Exception("Invalid URL - no schema supplied: " + url_);                        
        }
        
        schema_ = std::move(tokens[0]);
        auto other = splitString(tokens[1], "/", 1);

        host_ = std::move(other[0]);        
        path_ = "/";

        if (other.size() == 2)
        {
            path_.append(std::move(other[1]));
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

std::ostream &operator<<(std::ostream &os, const Url &url)
{
    os << url.url_;
    return os;
}
    
} // namespace requests

#endif /* URL_H */
