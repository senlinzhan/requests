![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg) 
# requests
基于 Boost.Asio 实现的 HTTP 客户端网络库

## 快速上手
```C++
#include "Request.hpp"
#include <iostream>

int main()
{
    requests::Request request;
    requests::Url url("http://www.baidu.com");
     
    auto resp = request.get(url);
    std::cout << url << std::endl;                             // http://www.baidu.com
    std::cout << resp.statusCode() << std::endl;               // 200
    std::cout << resp.headers()["Content-Type"] << std::endl;  // text/html
		
    return 0;
}
```
