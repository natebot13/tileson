cmake_minimum_required(VERSION 3.11)

set(TILESON_PROJECT_VERSION_MAJOR 1)
set(TILESON_PROJECT_VERSION_MINOR 3)
set(TILESON_PROJECT_VERSION_PATCH 1)
string(JOIN "." TILESON_VERSION ${TILESON_PROJECT_VERSION_MAJOR} ${TILESON_PROJECT_VERSION_MINOR} ${TILESON_PROJECT_VERSION_PATCH})

project(
  tileson
  VERSION ${TILESON_VERSION}
  DESCRIPTION "Modern C++ json Tiled map parser"
  HOMEPAGE_URL "https://github.com/SSBMTonberry/tileson"
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)

# Add tileson target

include(GNUInstallDirs)

add_library(tileson INTERFACE)
add_library(tileson::tileson ALIAS tileson)

target_include_directories(tileson INTERFACE 
    $<BUILD_INTERFACE:${tileson_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(tileson INTERFACE cxx_std_17)


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
option(CLEAR_TSON_CMAKE_CACHE "Clear CMake cache for variables. Activate if you are working on the code. Otherwise: keep it off" OFF)

option(USE_LEGACY_CLANG_FOR_TESTS "Only use when you have an old version of Homebrew's Clang on OSX" OFF)

configure_file (
        "${PROJECT_SOURCE_DIR}/TilesonConfig.h.in"
        "${PROJECT_SOURCE_DIR}/TilesonConfig.h"
)

include_directories(${PROJECT_SOURCE_DIR}/external_libs/)

# install pkgconfig files

set(tileson_PKGCONFIG ${CMAKE_CURRENT_BINARY_DIR}/tileson.pc)

configure_file(
    ${tileson_SOURCE_DIR}/cmake/in/tileson.pc.in
    ${tileson_PKGCONFIG}
    @ONLY
)

install(
    FILES ${tileson_PKGCONFIG}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# install tileson

include(CMakePackageConfigHelpers)

install(
    TARGETS tileson
    EXPORT tilesonTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

write_basic_package_version_file(
    tilesonConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${tileson_SOURCE_DIR}/cmake/in/tilesonConfig.cmake.in
    tilesonConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/tileson/cmake
)

export(
    EXPORT tilesonTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/tilesonTargets.cmake
    NAMESPACE tileson::
)

install(
    EXPORT tilesonTargets
    FILE tilesonTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/tileson/cmake
    NAMESPACE tileson::
)

install(
    FILES
        ${PROJECT_BINARY_DIR}/tilesonConfig.cmake
        ${PROJECT_BINARY_DIR}/tilesonConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/tileson/cmake
)

install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

export(PACKAGE tileson)

# tests and examples

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