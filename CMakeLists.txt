cmake_minimum_required(VERSION 3.18)
project(hot_teacup
        VERSION 3.2.0
        DESCRIPTION "C++17 library for parsing HTTP requests data received over the FastCGI connection and forming HTTP responses."
        )
include(GNUInstallDirs)
include(external/seal_lake)

SealLake_Bundle(
        NAME hot_teacup_sfun
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG v3.1.1
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace http::sfun"
)

set(SRC
    src/cookie.cpp
    src/cookie_view.cpp
    src/form.cpp
    src/form_view.cpp
    src/header.cpp
    src/header_view.cpp
    src/query.cpp
    src/query_view.cpp
    src/request.cpp
    src/request_view.cpp
    src/response.cpp
    src/response_view.cpp
)

set(PUBLIC_HEADERS
    include/hot_teacup/cookie.h
    include/hot_teacup/form.h
    include/hot_teacup/header.h
    include/hot_teacup/query.h
    include/hot_teacup/request.h
    include/hot_teacup/response.h
    include/hot_teacup/types.h
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

SealLake_OptionalBuildSteps(tests)