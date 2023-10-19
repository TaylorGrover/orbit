cmake_minimum_required(VERSION 3.16...3.20 FATAL_ERROR)

project(Orbit VERSION 3.4.0 LANGUAGES C CXX)

if (POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

if (POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

string(COMPARE EQUAL "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" Orbit_STANDALONE)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(Orbit_BUILD_TESTS "Build the GLFW test programs" ${Orbit_STANDALONE})
option(Orbit_BUILD_EXAMPLES "Build the GLFW examples" ${Orbit_STANDALONE})
option(Orbit_BUILD_DOCS "Build the GLFW documentation" ON)
option(Orbit_INSTALL "Generate installation target" ON)

include(GNUInstallDirs)
include(CMakeDependentOption)
include(CTest) # Include CTest https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html

# Include directory visibility for tests and examples
include_directories("${Orbit_SOURCE_DIR}/deps" "${Orbit_SOURCE_DIR}/include")

if (Orbit_USE_OSMESA)
    message(FATAL_ERROR "Orbit_USE_OSMESA has been removed; set the Orbit_PLATFORM init hint")
endif()

cmake_dependent_option(Orbit_BUILD_WIN32 "Build support for Win32" ON "WIN32" OFF)
cmake_dependent_option(Orbit_BUILD_COCOA "Build support for Cocoa" ON "APPLE" OFF)
cmake_dependent_option(Orbit_BUILD_X11 "Build support for X11" ON "UNIX;NOT APPLE" OFF)
cmake_dependent_option(Orbit_BUILD_WAYLAND "Build support for Wayland"
    "${Orbit_USE_WAYLAND}" "UNIX;NOT APPLE" OFF)

cmake_dependent_option(Orbit_USE_HYBRID_HPG "Force use of high-performance GPU on hybrid systems" OFF
                       "WIN32" OFF)
cmake_dependent_option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" ON
                       "MSVC" OFF)

set(Orbit_LIBRARY_TYPE "${Orbit_LIBRARY_TYPE}" CACHE STRING
    "Library type override for GLFW (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

if (Orbit_LIBRARY_TYPE)
    if (Orbit_LIBRARY_TYPE STREQUAL "SHARED")
        set(Orbit_BUILD_SHARED_LIBRARY TRUE)
    else()
        set(Orbit_BUILD_SHARED_LIBRARY FALSE)
    endif()
else()
    set(Orbit_BUILD_SHARED_LIBRARY ${BUILD_SHARED_LIBS})
endif()

list(APPEND CMAKE_MODULE_PATH "${Orbit_SOURCE_DIR}/CMake/modules")

find_package(Threads REQUIRED)
#find_package(Qt5 COMPONENTS Widgets REQUIRED)
#find_package(Qt5 COMPONENTS Core REQUIRED)

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")

#set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_COVERAGE "RELEASE")

if (Orbit_BUILD_DOCS)
    set(DOXYGEN_SKIP_DOT TRUE)
    find_package(Doxygen)
endif()

#--------------------------------------------------------------------
# Report backend selection
#--------------------------------------------------------------------
if (Orbit_BUILD_WIN32)
    message(STATUS "Including Win32 support")
endif()
if (Orbit_BUILD_COCOA)
    message(STATUS "Including Cocoa support")
endif()
if (Orbit_BUILD_WAYLAND)
    message(STATUS "Including Wayland support")
endif()
if (Orbit_BUILD_X11)
    message(STATUS "Including X11 support")
endif()

#--------------------------------------------------------------------
# Apply Microsoft C runtime library option
# This is here because it also applies to tests and examples
#--------------------------------------------------------------------
if (MSVC AND NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
    if (CMAKE_VERSION VERSION_LESS 3.15)
        foreach (flag CMAKE_C_FLAGS
                      CMAKE_C_FLAGS_DEBUG
                      CMAKE_C_FLAGS_RELEASE
                      CMAKE_C_FLAGS_MINSIZEREL
                      CMAKE_C_FLAGS_RELWITHDEBINFO)

            if (flag MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
            if (flag MATCHES "/MDd")
                string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
            endif()

        endforeach()
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

#--------------------------------------------------------------------
# Create generated files
#--------------------------------------------------------------------
include(CMakePackageConfigHelpers)

set(Orbit_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/glfw3")

#--------------------------------------
# Libraries to include in compilation
#--------------------------------------
set(CAMERA "${Orbit_SOURCE_DIR}/deps/camera.cpp"
           "${Orbit_SOURCE_DIR}/deps/camera.hpp")
set(GLAD_GL "${Orbit_SOURCE_DIR}/deps/glad/gl.h")
set(GLAD_GLES2 "${Orbit_SOURCE_DIR}/deps/glad/gles2.h")
set(GETOPT "${Orbit_SOURCE_DIR}/deps/getopt.h"
           "${Orbit_SOURCE_DIR}/deps/getopt.c")
set(ENTITY "${Orbit_SOURCE_DIR}/deps/entity.hpp"
           "${Orbit_SOURCE_DIR}/deps/entity.cpp")
set(INPUT "${Orbit_SOURCE_DIR}/deps/input.hpp"
          "${Orbit_SOURCE_DIR}/deps/input.cpp")
set(SHADER "${Orbit_SOURCE_DIR}/deps/shader.h"
           "${Orbit_SOURCE_DIR}/deps/shader.cpp")
set(SPHERE "${Orbit_SOURCE_DIR}/deps/sphere.cpp")
set(STB_IMG "${Orbit_SOURCE_DIR}/deps/stb_image.h")
set(TINYCTHREAD "${Orbit_SOURCE_DIR}/deps/tinycthread.h"
                "${Orbit_SOURCE_DIR}/deps/tinycthread.c")


configure_package_config_file(CMake/glfw3Config.cmake.in
                              src/glfw3Config.cmake
                              INSTALL_DESTINATION "${Orbit_CONFIG_PATH}"
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(src/glfw3ConfigVersion.cmake
                                 VERSION ${Orbit_VERSION}
                                 COMPATIBILITY SameMajorVersion)

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
add_subdirectory(src)

if (Orbit_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (Orbit_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (DOXYGEN_FOUND AND Orbit_BUILD_DOCS)
    add_subdirectory(docs)
endif()

#--------------------------------------------------------------------
# Install files other than the library
# The library is installed by src/CMakeLists.txt
#--------------------------------------------------------------------
if (Orbit_INSTALL)
    install(DIRECTORY include/GLFW DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN glfw3.h PATTERN glfw3native.h)

    install(FILES "${Orbit_BINARY_DIR}/src/glfw3Config.cmake"
                  "${Orbit_BINARY_DIR}/src/glfw3ConfigVersion.cmake"
            DESTINATION "${Orbit_CONFIG_PATH}")

    install(EXPORT glfwTargets FILE glfw3Targets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
            DESTINATION "${Orbit_CONFIG_PATH}")
    install(FILES "${Orbit_BINARY_DIR}/src/glfw3.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

    if (DOXYGEN_FOUND AND Orbit_BUILD_DOCS)
        install(DIRECTORY "${Orbit_BINARY_DIR}/docs/html"
                DESTINATION "${CMAKE_INSTALL_DOCDIR}")
    endif()

    # Only generate this target if no higher-level project already has
    if (NOT TARGET uninstall)
        configure_file(CMake/cmake_uninstall.cmake.in
                       cmake_uninstall.cmake IMMEDIATE @ONLY)

        add_custom_target(uninstall
                          "${CMAKE_COMMAND}" -P
                          "${Orbit_BINARY_DIR}/cmake_uninstall.cmake")
        set_target_properties(uninstall PROPERTIES FOLDER "GLFW3")
    endif()
endif()

