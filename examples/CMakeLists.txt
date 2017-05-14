INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/requests)


add_executable(SyncRequest SyncRequest.cpp)
target_link_libraries(SyncRequest requests)
target_link_libraries(SyncRequest ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(SyncRequest ${Boost_SYSTEM_LIBRARY})


add_executable(AsyncRequest AsyncRequest.cpp)
target_link_libraries(AsyncRequest requests)
target_link_libraries(AsyncRequest ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(AsyncRequest ${Boost_SYSTEM_LIBRARY})
