enable_testing()

include_directories(${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

link_directories(${PROJECT_BINARY_DIR}/requests)

add_executable(UrlTests UrlTests.cpp)

target_link_libraries(UrlTests requests)
target_link_libraries(UrlTests ${GTEST_BOTH_LIBRARIES})
target_link_libraries(UrlTests ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(UrlTests ${Boost_SYSTEM_LIBRARY})

add_test(Test UrlTests)
