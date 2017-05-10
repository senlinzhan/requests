#ifndef ASYNCREQUEST_H
#define ASYNCREQUEST_H

#include "Exception.hpp"
#include "Response.hpp"
#include "Url.hpp"
#include "Context.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <thread>

namespace requests {
   
class AsyncRequest
{
public:
    using IOService  = boost::asio::io_service;
    using Resolver   = boost::asio::ip::tcp::resolver;
    using Socket     = boost::asio::ip::tcp::socket;
    using ErrorCode  = boost::system::error_code;
    using Buffer     = boost::asio::streambuf;
    using String     = std::string;   
    using Work       = IOService::work;
    using StringMap  = std::unordered_map<String, String>;
    
    AsyncRequest()
        : service_(),
          resolver_(service_),
          work_(new Work(service_))
    {
        thread_.reset(new std::thread([this] ()
                                      {
                                          service_.run();
                                      }));
    }

    ~AsyncRequest()
    {
        work_.reset(nullptr);
        thread_->join();        
    }
    
    // disable the copy operations
    AsyncRequest(const AsyncRequest &) = delete;
    AsyncRequest &operator=(const AsyncRequest &) = delete;

    // enable the move operations
    AsyncRequest(AsyncRequest &&) = default;
    AsyncRequest &operator=(AsyncRequest &&) = default;

    void get(const Url &url, const UserCallback &callback)
    {
        // empty query parameters
        StringMap params;
        
        return get(url, params, callback);
    }
    
    void get(const Url &url, const StringMap &params, const UserCallback &callback)
    {
        Resolver::query query(url.host(), url.port());
        
        auto context = std::make_shared<Context>(service_, url, Context::Method::Get, params, callback);        
        
        resolver_.async_resolve(query,
                                [this, context] (const ErrorCode &err, Resolver::iterator iter)
                                {
                                    handleResolve(err, iter, context);
                                });                
    }    

    void post(const Url &url, const StringMap &data, const UserCallback &callback)
    {
        Resolver::query query(url.host(), url.port());

        auto context = std::make_shared<Context>(service_, url, Context::Method::Post, data, callback);        
        
        resolver_.async_resolve(query,
                                [this, context] (const ErrorCode &err, Resolver::iterator iter)
                                {
                                    handleResolve(err, iter, context);
                                });                        
    }
    
private:    
    void handleResolve(const ErrorCode &err, Resolver::iterator iter, ContextPtr context)
    {
        if (err)
        {
            throw Exception("Resolve domain name error: " + err.message());
        }
        
        auto endpoint = iter->endpoint();
        auto &sock = context->socket();

        ++iter;        
        sock.async_connect(endpoint,
                           [this, iter, context] (const ErrorCode &err)
                           {
                               handleConnect(err, iter, context);
                           });
    }


    void handleConnect(const ErrorCode &err, Resolver::iterator iter, ContextPtr context)
    {
        auto &sock = context->socket();
        auto &buffer = context->reqBuff();
        
        if (err)
        {
            Resolver::iterator nullIter;            
            if (iter == nullIter)
            {
                throw Exception("Resolve domain name error: " + err.message());
            }
            sock.close();
            
            ErrorCode nullErr;
            handleResolve(nullErr, iter, context);            
        }
        else
        {
            boost::asio::async_write(sock, buffer,
                                     [this, context] (const ErrorCode &err,
                                                      std::size_t /* bytesTransferred */) 
                                     {
                                         handleWrite(err, context);
                                     });            
        }
    }
    
    void handleWrite(const ErrorCode &err, ContextPtr context)
    {
        if (err)
        {
            throw Exception("Send request headers to server error: " + err.message());
        }

        auto &sock = context->socket();
        auto &respBuffer = context->respBuff();

        // shutdown on write
        sock.shutdown(Socket::shutdown_send);
        
        boost::asio::async_read_until(sock, respBuffer, "\r\n\r\n",
                                      [this, context] (const ErrorCode &err,
                                                       std::size_t /* bytesTransferred */)
                                      {
                                          handleReadHeaders(err, context);
                                      });
    }

    void handleReadHeaders(const ErrorCode &err, ContextPtr context)
    {
        if (err)
        {
            throw Exception("Read response headers error: " + err.message());
        }

        auto &sock = context->socket();
        auto &respBuffer = context->respBuff();

        // NOTE: str may contain mutiple "\r\n\r\n"
        auto str = bufferToString(respBuffer);
        auto parts = splitString(str, "\r\n\r\n", 1);
        
        String headers, content;
        headers = std::move(parts[0]);        
        if (parts.size() == 2)
        {
            content = std::move(parts[1]);
        }
        
        context->setHeaders(std::move(headers));
        context->appendContent(content);
                
        boost::asio::async_read(sock,
                                respBuffer,
                                boost::asio::transfer_at_least(1),
                                [this, context] (const ErrorCode &err,
                                                 std::size_t /* bytes_transferred */)
                                {
                                    handleReadBody(err, context);
                                });        
    }

    void handleReadBody(const ErrorCode &err, ContextPtr context)
    {
        if (err)
        {
            if (err != boost::asio::error::eof)
            {
                throw Exception("Read response body error: " + err.message());                            
            }
            
            context->handleResponse();

            return;
        }

        auto &sock = context->socket();
        auto &respBuffer = context->respBuff();

        auto content = bufferToString(respBuffer);
        context->appendContent(content);
        
        boost::asio::async_read(sock, respBuffer,
                                boost::asio::transfer_at_least(1),
                                [this, context] (const ErrorCode &err,
                                                 std::size_t /* bytes_transferred */)
                                {
                                    handleReadBody(err, context);
                                });
    }

    IOService                     service_;
    Resolver                      resolver_;
    std::unique_ptr<Work>         work_;
    std::unique_ptr<std::thread>  thread_;
};

} // namespace requests

#endif /* ASYNCREQUEST_H */
