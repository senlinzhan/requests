#ifndef ASYNCREQUEST_H
#define ASYNCREQUEST_H

#include "Callback.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include <thread>

namespace requests {

class Url;
class Context;
    
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
    using ContextPtr = std::shared_ptr<Context>;
    
    AsyncRequest();

    ~AsyncRequest();

    // disable the copy operations
    AsyncRequest(const AsyncRequest &) = delete;
    AsyncRequest &operator=(const AsyncRequest &) = delete;

    // enable the move operations
    AsyncRequest(AsyncRequest &&) = default;
    AsyncRequest &operator=(AsyncRequest &&) = default;

    void get(const Url &url, const UserCallback &callback);
    
    void get(const Url &url, const StringMap &params, const UserCallback &callback);

    void post(const Url &url, const StringMap &data, const UserCallback &callback);
    
private:    
    void handleResolve(const ErrorCode &err, Resolver::iterator iter, ContextPtr context);

    void handleConnect(const ErrorCode &err, Resolver::iterator iter, ContextPtr context);
    
    void handleWrite(const ErrorCode &err, ContextPtr context);

    void handleReadHeaders(const ErrorCode &err, ContextPtr context);

    void handleReadBody(const ErrorCode &err, ContextPtr context);

    IOService                     service_;
    Resolver                      resolver_;
    std::unique_ptr<Work>         work_;
    std::unique_ptr<std::thread>  thread_;
};

} // namespace requests

#endif /* ASYNCREQUEST_H */
