#ifndef CALLBACK_H
#define CALLBACK_H

#include <functional>

namespace requests {
    
class Response;
class Exception;
    
using UserCallback  = std::function<void (Response &)>;
using ErrorCallback = std::function<void (Exception &)>;
    
};


#endif /* CALLBACK_H */
