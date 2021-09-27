cmake_minimum_required(VERSION 3.18)

if(NOT DEFINED HOT_TEACUP_SUBPROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(HOT_TEACUP_SUBPROJECT OFF)
    else()
        set(HOT_TEACUP_SUBPROJECT ON)
    endif()
endif()

project(hot_teacup
        VERSION 1.0.0
        DESCRIPTION "C++17 library for parsing HTTP requests data received over the FastCGI connection and forming HTTP responses.")

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(sfun
        GIT_REPOSITORY "https://github.com/kamchatka-volcano/sfun.git"
        GIT_TAG fe5b60f3fd7a545515ca2e50a406c6bc2c924ac7
        GIT_SHALLOW    ON
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(sfun)

set(SRC
    src/cookie.cpp
        src/form.cpp
    src/header.cpp
    src/query.cpp
    src/request.cpp
    src/response.cpp
    src/types.cpp)
    
set(PUBLIC_HEADERS
    "include/hot_teacup/cookie.h"
    "include/hot_teacup/form.h"
    "include/hot_teacup/header.h"
    "include/hot_teacup/query.h"
    "include/hot_teacup/request.h"
    "include/hot_teacup/response.h"
    "include/hot_teacup/types.h")

include(CheckPIESupported)
check_pie_supported()

add_library(hot_teacup STATIC ${SRC})

target_compile_features(hot_teacup PUBLIC cxx_std_17)
set_target_properties(hot_teacup PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(hot_teacup PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(hot_teacup PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
set_target_properties(hot_teacup PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(hot_teacup PRIVATE sfun)
target_include_directories(hot_teacup INTERFACE include/)
target_include_directories(hot_teacup PRIVATE include/)
target_include_directories(hot_teacup PRIVATE src)

option(ENABLE_TESTS "Enable tests" OFF)
if (ENABLE_TESTS AND NOT HOT_TEACUP_SUBPROJECT)
    enable_testing()
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)
if (NOT HOT_TEACUP_SUBPROJECT)
install(TARGETS hot_teacup
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hot_teacup)
endif()