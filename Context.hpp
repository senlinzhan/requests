#ifndef CONTEXT_H
#define CONTEXT_H

#include "Exception.hpp"
#include "Response.hpp"
#include "Url.hpp"

#include <boost/asio.hpp>

namespace requests {

class Context;

using ContextPtr = std::shared_ptr<Context>;
using UserCallback = std::function<void(Response &)>;
    
class Context
{
public:
    using IOService  = boost::asio::io_service;
    using Socket     = boost::asio::ip::tcp::socket;
    using Buffer     = boost::asio::streambuf;
    using String     = std::string;

    
    Context(IOService &service, const Url &url, const UserCallback &callback)
        : sock_(service),
          url_(url),
          callback_(callback)
    {
        std::ostream reqStream(&requestBuff_);
        
        reqStream << "GET " << url.path() << " HTTP/1.0\r\n";
        reqStream << "Host: " << url.host() << "\r\n";
        reqStream << "Accept: */*\r\n";
        reqStream << "Connection: close\r\n\r\n";            
    }
    
    Socket &socket()
    {
        return sock_;
    }

    const Socket &socket() const
    {
        return sock_;
    }
    
    Url &url()
    {
        return url_;
    }

    const Url &url() const
    {
        return url_;
    }
    
    Buffer &reqBuff()
    {
        return requestBuff_;
    }
    
    const Buffer &reqBuff() const
    {
        return requestBuff_;
    }
    
    Buffer &respBuff()
    {
        return responseBuff_;
    }

    const Buffer &respBuff() const
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
    
    void appendContent(const String content)
    {
        content_ += content;
    }
    
    void handleResponse()
    {
        Response resp(std::move(headers_), std::move(content_));
        callback_(resp);
    }
    
private:
    Socket        sock_;
    Url           url_;
    UserCallback  callback_;
    Buffer        requestBuff_;
    Buffer        responseBuff_;
    String        headers_;
    String        content_;
};

}  // namespace requests

#endif /* CONTEXT_H */
