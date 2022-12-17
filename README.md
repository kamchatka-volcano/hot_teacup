# ☕hot_teacup
[![build & test (clang, gcc, MSVC)](https://github.com/kamchatka-volcano/hot_teacup/actions/workflows/build_and_test.yml/badge.svg?branch=master)](https://github.com/kamchatka-volcano/hot_teacup/actions/workflows/build_and_test.yml)

**hot_teacup** is a C++17 library for parsing HTTP request data received over a FastCGI connection and forming HTTP responses. It supports reading HTTP headers, cookies, query strings, URL encoded forms, and multipart forms. The library is designed for use with the FastCGI protocol, so input data is expected to be percent-decoded by the web server. The headers are self-explanatory, so there is no documentation. If you need examples of usage, you can also check the unit tests.

### Installation
Download and link the library from your project's CMakeLists.txt:
```
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(hot_teacup
    GIT_REPOSITORY "https://github.com/kamchatka-volcano/hot_teacup.git"
    GIT_TAG "origin/master"
)

#uncomment if you need to install hot_teacup with your target
#set(INSTALL_HOT_TEACUP ON)
FetchContent_MakeAvailable(hot_teacup)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE hot_teacup::hot_teacup)
```

To install the library system-wide, use the following commands:
```
git clone https://github.com/kamchatka-volcano/hot_teacup.git
cd hot_teacup
cmake -S . -B build
cmake --build build
cmake --install build
```

After installation, you can use the `find_package()` command to make the installed library available inside your project:
```
find_package(hot_teacup 1.0.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE hot_teacup::hot_teacup)
```

### Running tests
```
cd hot_teacup
cmake -S . -B build -DENABLE_TESTS=ON
cmake --build build 
cd build/tests && ctest
```

### License
**hot_teacup** is licensed under the [MS-PL license](/LICENSE.md)  
