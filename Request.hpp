#ifndef REQUESTS_H
#define REQUESTS_H

#include "Exception.hpp"
#include "Response.hpp"
#include "Url.hpp"
#include "Utils.hpp"
#include "Context.hpp"

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
        Context context(service_, url, Context::Method::Get, params);
        makeHttpRequest(context);        
        
        return context.getResponse();
    }

    void get(const Url &url, const StringMap &params, const UserCallback &callback)
    {
        Context context(service_, url, Context::Method::Get, params, callback);
        makeHttpRequest(context);
        
        context.handleResponse();
    }
    
    Response get(const Url &url)
    {
        StringMap emptyParams;

        return get(url, emptyParams);
    }
    
    Response post(const Url &url, const StringMap &data)
    {
        Context context(service_, url, Context::Method::Post, data);
        makeHttpRequest(context);        
        
        return context.getResponse();
    }

    void post(const Url &url, const StringMap &data, const UserCallback &callback)
    {
        Context context(service_, url, Context::Method::Post, data, callback);
        makeHttpRequest(context);
        
        context.handleResponse();
    }
            
private:
    // make HTTP request and read response
    void makeHttpRequest(Context &context)
    {
        syncResolve(context);
        syncSendRequest(context);
        syncReadResponse(context);        
    }

    // resolve domain name synchronously
    void syncResolve(Context &context)
    {
        auto &url = context.url();
        auto &sock = context.socket();

        Resolver::query query(url.host(), url.port());
         
        ErrorCode err;
        Resolver::iterator iter;        
        Resolver::iterator nullIter;

        for (iter = resolver_.resolve(query); iter != nullIter; ++iter)
        {
            auto endpoint = iter->endpoint();
            
            sock.open(endpoint.protocol(), err);
            if (err)
            {
                continue;
            }            

            sock.connect(endpoint, err);
            if (err)
            {
                sock.close();
                continue;
            }

            break;
        }

        if (iter == nullIter)
        {
            throw Exception("Resolve host " + url.host() + " error: " + err.message());                        
        }        
    }

    // send HTTP request headers and body synchronously
    void syncSendRequest(Context &context)
    {
        auto &sock = context.socket();
        auto &reqBuff = context.reqBuff();
        
        boost::asio::write(sock, reqBuff);

        // shutdown on write
        sock.shutdown(Socket::shutdown_send);        
    }

    // read HTTP response synchronously
    void syncReadResponse(Context &context)
    {
        auto &sock = context.socket();
        auto &respBuff = context.respBuff();

        // NOTE: the result may contain mutiple "\r\n\r\n"
        boost::asio::read_until(sock, respBuff, "\r\n\r\n");
        
        auto str = bufferToString(respBuff);
        auto parts = splitString(str, "\r\n\r\n", 1);
        
        auto headers = std::move(parts[0]);
        context.setHeaders(std::move(headers));

        
        if (parts.size() == 2)
        {
            auto content = parts[1];
            context.appendContent(content);
        }
                
        ErrorCode readErr;
        while (boost::asio::read(sock, respBuff,
                                 boost::asio::transfer_at_least(1),
                                 readErr))
        {
            if (readErr)
            {
                break;
            }
            auto content = bufferToString(respBuff);
            context.appendContent(content);
        }
        
        if (readErr && readErr != boost::asio::error::eof)
        {
            throw Exception("Read response error: " + readErr.message());
        }
    }
        
    IOService service_;
    Resolver  resolver_;
};

} // namespace requests

#endif /* REQUESTS_H */
