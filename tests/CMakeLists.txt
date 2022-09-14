cmake_minimum_required(VERSION 3.18)
project(test_hot_teacup)

SealLake_GoogleTest(test_hot_teacup
        SOURCES
            test_request.cpp
            test_response.cpp
            test_cookie.cpp
            test_header.cpp
            test_query.cpp
            test_form.cpp
        LIBRARIES
            hot_teacup::hot_teacup
)
