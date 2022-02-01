cmake_minimum_required(VERSION 3.11)
project(tileson CXX)

set(CMAKE_CXX_STANDARD 17)
set(TILESON_PROJECT_VERSION_MAJOR 1)
set(TILESON_PROJECT_VERSION_MINOR 3)
set(TILESON_PROJECT_VERSION_PATCH 1)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTILESON_DEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG 1)
    message(DEBUGGIN')
else()
    set(RELEASE 1)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX 1)
    set(WINDOWS 0)
    set(APPLE 0)
    set(MSVC 0)
    set(MINGW 0)
elseif(APPLE)
    link_directories(/usr/local/opt/llvm/lib/)
    set(LINUX 0)
    set(WINDOWS 0)
    set(APPLE 1)
    set(MSVC 0)
    set(MINGW 0)
elseif(WIN32)
    set(LINUX 0)
    set(WINDOWS 1)
    set(APPLE 0)
    if(MSVC)
        set(MSVC 1)
        set(MINGW 0)
    elseif(MINGW)
        set(MINGW 1)
        set(MSVC 0)
    endif()
else()
    set(LINUX 0)
    set(WINDOWS 0)
    set(APPLE 0)
    set(MSVC 0)
    set(MINGW 0)
endif()


#option(BUILD_SHARED_LIBS "Build shared libraries." OFF)
option(BUILD_TESTS "Build unit tests." ON)
option(BUILD_EXAMPLES "Build examples." OFF)
option(CLEAR_TSON_CMAKE_CACHE "Clear CMake cache for variables. Activate if you are working on the code. Otherwise: keep it off" ON)

option(USE_LEGACY_CLANG_FOR_TESTS "Only use when you have an old version of Homebrew's Clang on OSX" OFF)

#if(APPLE)
#    option(DISABLE_CPP17_FILESYSTEM "Disables cpp17 std::filesystem for anything file related" ON)
#else()
#    option(DISABLE_CPP17_FILESYSTEM "Disables cpp17 std::filesystem for anything file related" OFF)
#endif()

# option(DISABLE_CPP17_FILESYSTEM "Disables cpp17 std::filesystem for anything file related" OFF)

#if(DISABLE_CPP17_FILESYSTEM)
#    set(DISABLE_CPP17_FILESYSTEM 1)
#else()
#    set(DISABLE_CPP17_FILESYSTEM 0)
#endif()

configure_file (
        "${PROJECT_SOURCE_DIR}/TilesonConfig.h.in"
        "${PROJECT_SOURCE_DIR}/TilesonConfig.h"
)

include_directories(${PROJECT_SOURCE_DIR}/external_libs/)

# Add tileson target

include(GNUInstallDirs)

add_library(tileson INTERFACE)
add_library(tson ALIAS tileson)

target_include_directories(tileson INTERFACE 
    $<BUILD_INTERFACE:${tileson_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(tileson INTERFACE cxx_std_17)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    set(BUILD_SHARED_LIBS OFF) #build SFML with static libs
    #set(SFML_BUILD_AUDIO OFF)

    add_subdirectory(external_libs/source/sfml)
    add_subdirectory(examples)
endif()

if(CLEAR_TSON_CMAKE_CACHE)
    unset(BUILD_TESTS CACHE) #Must clear the cache to detect changes in this variable
    unset(BUILD_EXAMPLES CACHE) #Must clear the cache to detect changes in this variable
    unset(DISABLE_CPP17_FILESYSTEM CACHE)
endif()
unset(CLEAR_TSON_CMAKE_CACHE CACHE)