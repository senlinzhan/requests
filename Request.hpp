#ifndef REQUESTS_H
#define REQUESTS_H

#include "Response.hpp"
#include "Url.hpp"

#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <iostream>

namespace requests {
    
class Request
{
public:
    using IOService = boost::asio::io_service;
    using Resolver  = boost::asio::ip::tcp::resolver;
    using Socket    = boost::asio::ip::tcp::socket;
    using SocketPtr = std::unique_ptr<Socket>;
    using ErrorCode = boost::system::error_code;
    using Buffer    = boost::asio::streambuf;
    using String    = std::string;
    
    Request() = default;

    Response get(const Url &url)
    {
        Resolver resolver(service_);
        Resolver::query query(url.host(), "http");

        ErrorCode err;
        Resolver::iterator endpoint;        
        Resolver::iterator nullEndpoint;

        SocketPtr socket = nullptr;
        
        for (endpoint = resolver.resolve(query); endpoint != nullEndpoint; ++endpoint)
        {
            SocketPtr sock(new Socket(service_));
            sock->connect(*endpoint, err);            
            if (!err)
            {
                socket = std::move(sock);                
                break;
            }
        }

        if (socket == nullptr)
        { 
            throw Exception(err.message());            
        }

        sendRequestHeaders(socket, url);
        String headers = readResponseHeaders(socket);

        ErrorCode readErr;
        Buffer respBuff;
        String content;

        std::size_t n;
        while ((n = boost::asio::read(*socket, respBuff,
                                      boost::asio::transfer_at_least(1),
                                      readErr)))
        {
            if (readErr)
            {
                break;
            }
            content += bufferToString(respBuff, n);
        }
        
        if (readErr && readErr != boost::asio::error::eof)
        {
            throw Exception(err.message());
        }
       
        return {std::move(headers), std::move(content)};
    }
            
private:
    static void sendRequestHeaders(SocketPtr &socket, const Url &url)
    {
        Buffer reqBuff;
        std::ostream reqStream(&reqBuff);
        
        reqStream << "GET " << url.path() << " HTTP/1.0\r\n";
        reqStream << "Host: " << url.host() << "\r\n";
        reqStream << "Accept: */*\r\n";
        reqStream << "Connection: close\r\n\r\n";
        
        boost::asio::write(*socket, reqBuff);        
    }

    static String readResponseHeaders(SocketPtr &socket)
    {
        Buffer respBuff;
        std::size_t n = boost::asio::read_until(*socket, respBuff, "\r\n\r\n");
        return bufferToString(respBuff, n);        
    }

    static String bufferToString(Buffer &buffer, std::size_t n)
    {
        auto data = buffer.data();
        String str(boost::asio::buffers_begin(data), boost::asio::buffers_begin(data) + n);        
        return str;
    }
    
    IOService service_;
};

} // namespace requests

#endif /* REQUESTS_H */
