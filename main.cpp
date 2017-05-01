#include <boost/asio.hpp>

#include "Requests.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    boost::asio::io_service service;
    
    Request request(service);
    Url url("http://www.baidu.com");
     
    auto resp = request.get(url);
    std::cout << "status code: " << resp.statusCode() << std::endl;
    std::cout << "status message: " << resp.statusMessage() << std::endl;

    std::cout << "Headers: " << std::endl;
    auto headers = resp.headers();
    std::cout << headers << std::endl;
     
    return 0;
}
