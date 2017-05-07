#include "Request.hpp"
#include "Response.hpp"
#include "AsyncRequest.hpp"

#include <iostream>

void callback(requests::Response &resp)
{
    std::cout << "Status Code: " << resp.statusCode() << std::endl;
    std::cout << "Content-Type: " << resp.headers()["Content-Type"] << std::endl;    
}

int main(int argc, char *argv[])
{   
    requests::Request request;
    requests::Url url("http://www.baidu.com");

    try {
        auto resp = request.get(url);
        std::cout << "Make request to " << url << std::endl;
        std::cout << "Status Code: " << resp.statusCode() << std::endl;
        std::cout << "Content-Type: " << resp.headers()["Content-Type"] << std::endl;

        boost::asio::io_service service;
        requests::AsyncRequest asyncRequests(service);
        
        asyncRequests.get(url, callback);
        service.run();            
    }
    catch (const requests::Exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    
    return 0;
}
