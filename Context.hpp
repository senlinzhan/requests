#ifndef CONTEXT_H
#define CONTEXT_H

#include "Exception.hpp"
#include "Response.hpp"
#include "Url.hpp"

#include <assert.h>
#include <boost/asio.hpp>
#include <unordered_map>

namespace requests {

class Context;

using ContextPtr = std::shared_ptr<Context>;
using UserCallback = std::function<void (Response &)>;
    
class Context
{
public:
    using IOService  = boost::asio::io_service;
    using Socket     = boost::asio::ip::tcp::socket;
    using Buffer     = boost::asio::streambuf;
    using String     = std::string;
    using StringMap  = std::unordered_map<String, String>;

    enum class Method { Get, Post };

    Context(IOService &service, const Url &url, Method method, const StringMap &data)
        : Context(service, url, method, data, UserCallback())
    {        
    }        
    
    Context(IOService &service, const Url &url, Method method, const StringMap &data, const UserCallback &callback)
        : sock_(service),
          url_(url),
          callback_(callback),
          method_(method)
    {
        std::ostream reqStream(&requestBuff_);

        if (method_ == Method::Get)           
        {
            url_.addQueries(data);
            
            reqStream << "GET " << url_.pathAndQueries() << " HTTP/1.1\r\n";
            reqStream << "Host: " << url_.host() << "\r\n";
            reqStream << "Accept: */*\r\n";
            reqStream << "Connection: close\r\n\r\n";                        
        }
        else if (method_ == Method::Post)
        {
            auto requestBody = urlEncode(data);
            auto length = std::to_string(requestBody.size());
            
            reqStream << "POST " << url_.path() << " HTTP/1.1\r\n";
            reqStream << "Host: " << url_.host() << "\r\n";
            reqStream << "Accept: */*\r\n";
            reqStream << "Content-Type: application/x-www-form-urlencoded\r\n";
            reqStream << "Content-Length: " << length << "\r\n";
            reqStream << "Connection: close\r\n\r\n";

            reqStream << requestBody;
        }
    }

    // disable the copy operations
    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;

    // enable the move operations
    Context(Context &&) = default;
    Context &operator=(Context &&) = default;
    
    const Socket &socket() const
    {
        return sock_;
    }    

    const Url &url() const
    {
        return url_;
    }
    
    const Buffer &reqBuff() const
    {
        return requestBuff_;
    }
    
    const Buffer &respBuff() const
    {
        return responseBuff_;
    }

    Socket &socket()
    {
        return sock_;
    }    

    Url &url()
    {
        return url_;
    }
    
    Buffer &reqBuff()
    {
        return requestBuff_;
    }
    
    Buffer &respBuff()
    {
        return responseBuff_;
    }
    
    void setHeaders(const String &headers)
    {
        headers_ = headers;
    }
    
    void setHeaders(String &&headers) noexcept
    {
        headers_ = std::move(headers);
    }
    
    void appendContent(const String &content)
    {
        content_.append(content);
    }
    
    void handleResponse()
    {
        assert(callback_);
        
        Response resp(std::move(headers_), std::move(content_));
        callback_(resp);
    }    

    Response getResponse()
    {
        assert(!callback_);
        
        Response resp(std::move(headers_), std::move(content_));
        return resp;
    }
    
private:
    Socket        sock_;
    Url           url_;
    UserCallback  callback_;
    Method        method_;
    Buffer        requestBuff_;
    Buffer        responseBuff_;
    String        headers_;
    String        content_;
};

}  // namespace requests

#endif /* CONTEXT_H */
