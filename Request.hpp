#ifndef REQUESTS_H
#define REQUESTS_H

#include "Exception.hpp"
#include "Response.hpp"
#include "Url.hpp"
#include "Utils.hpp"

#include <memory>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <iostream>

namespace requests {    

class Request
{
public:
    using IOService   = boost::asio::io_service;
    using Resolver    = boost::asio::ip::tcp::resolver;
    using Socket      = boost::asio::ip::tcp::socket;
    using SocketPtr   = std::unique_ptr<Socket>;
    using ErrorCode   = boost::system::error_code;
    using Buffer      = boost::asio::streambuf;
    using String      = std::string;
    
    Request() = default;

    Response get(const Url &url)
    {
        Resolver resolver(service_);
        Resolver::query query(url.host(), "http");

        ErrorCode err;
        Resolver::iterator iter;        
        Resolver::iterator nullIter;

        SocketPtr socket = nullptr;
        
        for (iter = resolver.resolve(query); iter != nullIter; ++iter)
        {
            auto endpoint = iter->endpoint();
            SocketPtr sock(new Socket(service_, endpoint.protocol()));
            sock->connect(endpoint, err);            
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
        return readResponse(socket);
    }
            
private:
    void sendRequestHeaders(SocketPtr &socket, const Url &url)
    {
        Buffer reqBuff;
        std::ostream reqStream(&reqBuff);

        reqStream << "GET " << url.path() << " HTTP/1.0\r\n";
        reqStream << "Host: " << url.host() << "\r\n";
        reqStream << "Accept: */*\r\n";
        reqStream << "Connection: close\r\n\r\n";
        
        boost::asio::write(*socket, reqBuff);        
    }

    Response readResponse(SocketPtr &socket)
    {
        Buffer respBuff;

        // NOTE: the result may contain mutiple "\r\n\r\n"
        boost::asio::read_until(*socket, respBuff, "\r\n\r\n");
        
        auto str = bufferToString(respBuff);
        auto parts = splitString(str, "\r\n\r\n", 1);

        assert(parts.size() == 1 || parts.size() == 2);
        
        String headers, content;
        headers = std::move(parts[0]);        
        if (parts.size() == 2)
        {
            content = std::move(parts[1]);
        }
                
        ErrorCode readErr;
        while (boost::asio::read(*socket, respBuff,
                                 boost::asio::transfer_at_least(1),
                                 readErr))
        {
            if (readErr)
            {
                break;
            }
            content += bufferToString(respBuff);
        }
        if (readErr && readErr != boost::asio::error::eof)
        {
            throw Exception(readErr.message());
        }
       
        return {std::move(headers), std::move(content)};
    }
    
    IOService service_;
};

} // namespace requests

#endif /* REQUESTS_H */
