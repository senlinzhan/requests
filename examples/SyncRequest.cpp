#include "Url.hpp"
#include "Request.hpp"

#include <iostream>

int main()
{
    requests::Request request;
    requests::Url url("http://www.baidu.com");
    
    // 发起 HTTP 请求，阻塞
    auto resp = request.get(url);
	
    std::cout << url << std::endl;                             // http://www.baidu.com
    std::cout << resp.statusCode() << std::endl;               // 200
    std::cout << resp.headers()["Content-Type"] << std::endl;  // text/html
		
    return 0;
}
