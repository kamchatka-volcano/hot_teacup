cmake_minimum_required(VERSION 3.18)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(SRC
    test_request.cpp    
    test_response.cpp
    test_cookie.cpp
    test_header.cpp
    test_query.cpp)

add_executable(test_hot_teacup ${SRC})
add_test(NAME test_hot_teacup COMMAND test_hot_teacup)
target_include_directories(test_hot_teacup PRIVATE ../include/)
target_include_directories(test_hot_teacup PRIVATE ../src)
target_link_libraries(test_hot_teacup hot_teacup gtest gmock gtest_main pthread)
