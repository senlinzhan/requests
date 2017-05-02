#include "Request.hpp"
#include <iostream>

int main(int argc, char *argv[])
{   
    requests::Request request;
    requests::Url url("http://www.baidu.com");
     
    auto resp = request.get(url);
    std::cout << "Make request to " << url << std::endl;
    std::cout << "Status Code: " << resp.statusCode() << std::endl;
    std::cout << "Content-Type: " << resp.headers()["Content-Type"] << std::endl;
    
    return 0;
}
