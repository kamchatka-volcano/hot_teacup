cmake_minimum_required(VERSION 3.18)

project(hot_teacup
        VERSION 2.0.0
        DESCRIPTION "C++17 library for parsing HTTP requests data received over the FastCGI connection and forming HTTP responses."
)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(HOT_TEACUP_SUBPROJECT OFF)
else()
    set(HOT_TEACUP_SUBPROJECT ON)
endif()

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

include(CheckPIESupported)
check_pie_supported()

add_library(hot_teacup STATIC ${SRC})
add_library(hot_teacup::hot_teacup ALIAS hot_teacup)

target_compile_features(hot_teacup PUBLIC cxx_std_17)
set_target_properties(hot_teacup PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(hot_teacup PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(hot_teacup PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
set_target_properties(hot_teacup PROPERTIES POSITION_INDEPENDENT_CODE ON)

find_package(sfun 2.0.0 QUIET)
if (NOT sfun_FOUND)
    message("Configuration info: sfun library wasn't found on your system, downloading it from the GitHub repository")
    include(external/sfun)
endif()

target_link_libraries(hot_teacup PRIVATE "$<BUILD_INTERFACE:sfun::sfun>")
target_include_directories(hot_teacup PRIVATE include/)
target_include_directories(hot_teacup PRIVATE src)
target_include_directories(
        hot_teacup
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

option(ENABLE_TESTS "Enable tests" OFF)
if (ENABLE_TESTS AND NOT HOT_TEACUP_SUBPROJECT)
    enable_testing()
    add_subdirectory(tests)
endif()

option(INSTALL_HOT_TEACUP "Install hot_teacup library unconditionally" OFF)
if (NOT HOT_TEACUP_SUBPROJECT OR INSTALL_HOT_TEACUP)
    include(GNUInstallDirs)

    set(HOT_TEACUP_PACK_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/hot_teacup)
    install(TARGETS hot_teacup
            EXPORT hot_teacup-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hot_teacup
    )
    install(EXPORT hot_teacup-targets
            FILE hot_teacupTargets.cmake
            NAMESPACE hot_teacup::
            DESTINATION ${HOT_TEACUP_PACK_PATH}
    )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/hot_teacupConfigVersion.cmake
            COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/hot_teacupConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/hot_teacupConfig.cmake
            INSTALL_DESTINATION ${HOT_TEACUP_PACK_PATH}
    )

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/hot_teacupConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/hot_teacupConfigVersion.cmake
            DESTINATION ${HOT_TEACUP_PACK_PATH}
    )
endif()