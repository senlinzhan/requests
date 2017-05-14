#include "Context.hpp"
#include "Exception.hpp"
#include "Request.hpp"
#include "Url.hpp"
#include "Utils.hpp"

using namespace requests;

Request::Request()
    : service_(),
      resolver_(service_)
{        
}

Response Request::get(const Url &url, const StringMap &params)
{
    Context context(service_, url, Context::Method::Get, params);
    makeHttpRequest(context);        
        
    return context.getResponse();
}

void Request::get(const Url &url, const StringMap &params, const UserCallback &callback)
{
    Context context(service_, url, Context::Method::Get, params, callback);
    makeHttpRequest(context);
        
    context.handleResponse();
}
    
Response Request::get(const Url &url)
{
    StringMap emptyParams;

    return get(url, emptyParams);
}
    
Response Request::post(const Url &url, const StringMap &data)
{
    Context context(service_, url, Context::Method::Post, data);
    makeHttpRequest(context);        
        
    return context.getResponse();
}

void Request::post(const Url &url, const StringMap &data, const UserCallback &callback)
{
    Context context(service_, url, Context::Method::Post, data, callback);
    makeHttpRequest(context);
        
    context.handleResponse();
}
            
// make HTTP request and read response
void Request::makeHttpRequest(Context &context)
{
    syncResolve(context);
    syncSendRequest(context);
    syncReadResponse(context);        
}

// resolve domain name synchronously
void Request::syncResolve(Context &context)
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
void Request::syncSendRequest(Context &context)
{
    auto &sock = context.socket();
    auto &reqBuff = context.reqBuff();
        
    boost::asio::write(sock, reqBuff);

    // shutdown on write
    sock.shutdown(Socket::shutdown_send);        
}

// read HTTP response synchronously
void Request::syncReadResponse(Context &context)
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
