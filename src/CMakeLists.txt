set(SRCS
    Utils.cpp
    Url.cpp
    Response.cpp
    Context.cpp
    Request.cpp
    AsyncRequest.cpp)

add_library (requests STATIC ${SRCS})
