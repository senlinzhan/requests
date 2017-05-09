![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg) ![Build status](https://travis-ci.org/senlinzhan/requests.svg?branch=master)
# requests
基于 Boost.Asio 实现的 HTTP 客户端网络库，主要特性：
- 支持发送 GET 和 POST 请求
- 支持同步和异步发送请求

## 快速上手
#### 同步发送请求
```C++
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
```
#### 异步发送请求
```C++
#include "Response.hpp"
#include "AsyncRequest.hpp"
#include <iostream>

// 用户提供的回调函数
void callback(requests::Response &resp)
{
    std::cout << resp.statusCode() << std::endl;               // 200
    std::cout << resp.headers()["Content-Type"] << std::endl;  // text/html
}

int main(int argc, char *argv[])
{
    requests::AsyncRequest asyncRequest;
    requests::Url url("http://www.baidu.com");

    // 发起 HTTP 请求，非阻塞
    asyncRequest.get(url, callback);
    
    return 0;
}
```
