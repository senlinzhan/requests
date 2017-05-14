INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/requests)

ADD_EXECUTABLE(SyncRequest SyncRequest.cpp)

TARGET_LINK_LIBRARIES(SyncRequest requests ${Boost_SYSTEM_LIBRARY} pthread)

ADD_EXECUTABLE(AsyncRequest AsyncRequest.cpp)

TARGET_LINK_LIBRARIES(AsyncRequest requests ${Boost_SYSTEM_LIBRARY} pthread)
