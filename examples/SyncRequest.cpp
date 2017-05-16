#include <requests/Url.hpp>
#include <requests/Request.hpp>
#include <iostream>

int main()
{
    requests::Request request;
    
    // 发起 HTTP 请求，阻塞
    auto resp = request.get(requests::Url("http://www.baidu.com"));
	
    std::cout << resp.statusCode() << std::endl;               // 200
    std::cout << resp.headers()["Content-Type"] << std::endl;  // text/html
		
    return 0;
}
