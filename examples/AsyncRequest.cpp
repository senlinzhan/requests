#include <requests/AsyncRequest.hpp>
#include <requests/Exception.hpp>
#include <requests/Response.hpp>
#include <requests/Url.hpp>

#include <iostream>

// 用户提供的回调函数
void callback(requests::Response &resp)
{
    std::cout << resp.statusCode() << std::endl;               // 200
    std::cout << resp.headers()["Content-Type"] << std::endl;  // text/html
}

// 错误处理函数
void errorCallback(const requests::Exception &e)
{
    std::cout << e.what() << std::endl;    
}

int main()
{
    requests::AsyncRequest asyncRequest(4);     // 启动 4 个线程 
    requests::Url url("http://www.baidu.com");
    
    // 发起 HTTP 请求，非阻塞
    asyncRequest.get(url, callback, errorCallback);
    
    return 0;
}
