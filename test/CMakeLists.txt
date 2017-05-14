enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR})

link_directories(${PROJECT_BINARY_DIR}/requests)

add_executable(UrlTests UrlTests.cpp)

target_link_libraries(UrlTests requests gtest gtest_main ${Boost_SYSTEM_LIBRARY} pthread)

add_test(UrlTests UrlTests)
