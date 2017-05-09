#ifndef REQUESTS_H
#define REQUESTS_H

#include "Exception.hpp"
#include "Response.hpp"
#include "Url.hpp"
#include "Utils.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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
    using StringMap   = std::unordered_map<String, String>;

    enum class Method { Get, Post };
    
    Request()
        : service_(),
          resolver_(service_)
    {        
    }

    // disable the copy operations
    Request(const Request &) = delete;
    Request &operator=(const Request &) = delete;

    // enable the move operations
    Request(Request &&) = default;
    Request &operator=(Request &&) = default;

    Response get(const Url &url, const StringMap &params)
    {
        auto newUrl = url;
        newUrl.addQueries(params);
        
        return get(newUrl);
    }

    Response get(const Url &url)
    {
        auto socket = createSocket(url);        
        sendRequestHeaders(socket, url, Method::Get);
        
        return readResponse(socket);
    }
    
    Response post(const Url &url, const StringMap &data)
    {
        if (url.hasQueries())
        {
            throw Exception("Data to post could not in the url: " + url.toString());
        }
        
        auto socket = createSocket(url);
        sendRequestHeaders(socket, url, Method::Post);
        sendRequestBody(socket, data);
        return readResponse(socket);
    }
            
private:
    SocketPtr createSocket(const Url &url)
    {        
        Resolver::query query(url.host(), url.port());
         
        ErrorCode err;
        Resolver::iterator iter;        
        Resolver::iterator nullIter;

        SocketPtr socket = nullptr;
        for (iter = resolver_.resolve(query); iter != nullIter; ++iter)
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
            throw Exception("Resolve host " + url.host() + " error: " + err.message());            
        }
        
        return socket;
    }
     
    void sendRequestHeaders(SocketPtr &socket, const Url &url, Method method)
    {
        Buffer reqBuff;
        std::ostream reqStream(&reqBuff);

        if (method == Method::Get)
        {
            reqStream << "GET " << url.pathAndQueries() << " HTTP/1.1\r\n";
            reqStream << "Host: " << url.host() << "\r\n";
            reqStream << "Accept: */*\r\n";
            reqStream << "Connection: close\r\n\r\n";
            
            boost::asio::write(*socket, reqBuff);

            // shutdown on write
            socket->shutdown(Socket::shutdown_send);            
        }
        else if (method == Method::Post)
        {
            reqStream << "POST " << url.path() << " HTTP/1.1\r\n";
            reqStream << "Host: " << url.host() << "\r\n";
            reqStream << "Content-Type: application/x-www-form-urlencoded;charset=utf-8\r\n\r\n";

            boost::asio::write(*socket, reqBuff);            
        }
    }

    void sendRequestBody(SocketPtr &socket, const StringMap &data)
    {
        Buffer reqBuff;
        std::ostream reqStream(&reqBuff);

        reqStream << urlEncode(data);

        boost::asio::write(*socket, reqBuff);

        // shutdown on write
        socket->shutdown(Socket::shutdown_send);                    
    }
    
    Response readResponse(SocketPtr &socket)
    {
        Buffer respBuff;

        // NOTE: the result may contain mutiple "\r\n\r\n"
        boost::asio::read_until(*socket, respBuff, "\r\n\r\n");
        
        auto str = bufferToString(respBuff);
        auto parts = splitString(str, "\r\n\r\n", 1);
        
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
    Resolver  resolver_;
};

} // namespace requests

#endif /* REQUESTS_H */
