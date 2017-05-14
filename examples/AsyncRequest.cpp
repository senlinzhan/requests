#include <requests/Url.hpp>
#include <requests/Response.hpp>
#include <requests/AsyncRequest.hpp>
#include <iostream>

// 用户提供的回调函数
void callback(requests::Response &resp)
{
    std::cout << resp.statusCode() << std::endl;               // 200
    std::cout << resp.headers()["Content-Type"] << std::endl;  // text/html
}

int main()
{
    requests::AsyncRequest asyncRequest;
    requests::Url url("http://www.baidu.com");

    // 发起 HTTP 请求，非阻塞
    asyncRequest.get(url, callback);
    
    return 0;
}
