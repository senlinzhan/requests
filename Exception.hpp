#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

namespace requests {
    
class Exception : public std::exception
{
public:
    explicit Exception(const std::string &message) 
        : message_(message)
    {
    }

    explicit Exception(std::string &&message)
        : message_(message)
    {        
    }
    
    virtual const char *what() const noexcept override 
    {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

} // namespace requests

#endif /* EXCEPTION_H */
