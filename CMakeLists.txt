cmake_minimum_required(VERSION 3.18)
project(hot_teacup
        VERSION 3.3.0
        DESCRIPTION "C++17 library for parsing HTTP requests data received over the FastCGI connection and forming HTTP responses."
)
include(external/seal_lake)

SealLake_Bundle(
        NAME hot_teacup_sfun
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG develop
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace http::sfun"
)

set(SRC
        src/request_cookie.cpp
        src/request_cookie_view.cpp
        src/response_cookie.cpp
        src/response_cookie_view.cpp
        src/multipart_form.cpp
        src/multipart_form_view.cpp
        src/header.cpp
        src/header_view.cpp
        src/query.cpp
        src/query_view.cpp
        src/request.cpp
        src/request_view.cpp
        src/response.cpp
        src/response_view.cpp
        src/url_encoder.cpp
        src/url_encoded_form.cpp
        src/url_encoded_form_view.cpp
        src/utils.cpp
)

set(PUBLIC_HEADERS
        include/hot_teacup/request_cookie.h
        include/hot_teacup/cookie_view.h
        include/hot_teacup/multipart_form.h
        include/hot_teacup/multipart_form_view.h
        include/hot_teacup/url_encoded_form.h
        include/hot_teacup/url_encoded_form_view.h
        include/hot_teacup/header.h
        include/hot_teacup/header_view.h
        include/hot_teacup/query.h
        include/hot_teacup/query_view.h
        include/hot_teacup/request.h
        include/hot_teacup/request_view.h
        include/hot_teacup/response.h
        include/hot_teacup/response_view.h
        include/hot_teacup/types.h
        include/hot_teacup/detail/lazy_initialized.h
        include/hot_teacup/detail/view_or_owner_interface.h
        include/hot_teacup/multipart_form_impl.h
)

SealLake_StaticLibrary(
        SOURCES ${SRC}
        PUBLIC_HEADERS ${PUBLIC_HEADERS}
        COMPILE_FEATURES cxx_std_17
        PROPERTIES
            CXX_EXTENSIONS OFF
            VERSION ${PROJECT_VERSION}
            POSITION_INDEPENDENT_CODE ON
        LIBRARIES hot_teacup_sfun::hot_teacup_sfun
)

SealLake_OptionalSubProjects(tests)