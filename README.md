# ☕hot_teacup


**hot_teacup** - is a C++17 library for parsing HTTP requests data received over the FastCGI connection and forming HTTP responses.  
It supports reading of HTTP headers, cookies, query strings, URL encoded and multipart forms.   
The library is designed to be used with the FastCGI protocol, so input data is expected to be percent-decoded by the web server.
The headers are self-explanatory, so there's no documentation. You can also check the unit tests if you need examples of usage.

### Installation
Download and link the library from your project's CMakeLists.txt:
```
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(hot_teacup
    GIT_REPOSITORY "https://github.com/kamchatka-volcano/hot_teacup.git"
    GIT_TAG "origin/master"
)
FetchContent_MakeAvailable(hot_teacup)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE hot_teacup)
```

For the system-wide installation use these commands:
```
git clone https://github.com/kamchatka-volcano/hot_teacup.git
cd hot_teacup
cmake -S . -B build
cmake --build build
cmake --install build
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
