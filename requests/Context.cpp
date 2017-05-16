#include <requests/Context.hpp>
#include <requests/Utils.hpp>
#include <requests/Exception.hpp>

using namespace requests;

Context::Context(IOService &service, const Url &url, Method method, const StringMap &data)
    : Context(service, url, method, data, UserCallback())
{        
}        
    
Context::Context(IOService &service, const Url &url, Method method, const StringMap &data, const UserCallback &callback)
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
