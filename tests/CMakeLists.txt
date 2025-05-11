cmake_minimum_required(VERSION 3.18)
project(test_hot_teacup)

SealLake_GoogleTest(
        SOURCES
            test_request.cpp
            test_response.cpp
        test_response_cookie.cpp
        test_request_cookie.cpp
            test_header.cpp
            test_query.cpp
        test_multipart_form.cpp
        test_url_encoded_form.cpp
        LIBRARIES
            hot_teacup::hot_teacup
        hot_teacup_sfun::hot_teacup_sfun
)
