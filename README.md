![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg) ![Build status](https://travis-ci.org/senlinzhan/requests.svg?branch=master)
# requests
基于 Boost.Asio 实现的 HTTP 客户端网络库，主要特性：
- 支持发送 GET 和 POST 请求
- 支持同步和异步发送请求
- 允许用户设置回调函数
- URL 自动编码
## 快速上手
#### 同步发送请求
```C++
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
```
#### 异步发送请求
```C++
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
void errorCallback(requests::Exception &e)
{
    std::cout << e.what() << std::endl;
}

int main()
{
    requests::AsyncRequest asyncRequest;
    requests::Url url("http://www.baidu.com");
    
    // 发起 HTTP 请求，非阻塞
    asyncRequest.get(url, callback, errorCallback);
    
    return 0;
}
```
