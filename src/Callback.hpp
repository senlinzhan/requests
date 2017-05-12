#ifndef CALLBACK_H
#define CALLBACK_H

#include <functional>

namespace requests
{
    class Response;
    
    using UserCallback = std::function<void (Response &)>;
};


#endif /* CALLBACK_H */
