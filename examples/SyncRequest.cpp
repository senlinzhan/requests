#include <requests/Exception.hpp>
#include <requests/Request.hpp>
#include <requests/Url.hpp>

#include <iostream>

int main()
{
    requests::Request request;
    requests::Url url("http://www.baidu.com");
    
    try
    {
        // 发起 HTTP 请求，阻塞
        auto resp = request.get(url);
        std::cout << resp.statusCode() << std::endl;               // 200
        std::cout << resp.headers()["Content-Type"] << std::endl;  // text/html        
    }
    catch (requests::Exception &e)
    {
        std::cout << e.what() << std::endl;        
    }	
		
    return 0;
}
