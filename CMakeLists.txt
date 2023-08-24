cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)

project(Orbit VERSION 1.0.0 LANGUAGES C CXX)

if (POLICY CMP0069) 
   cmake_policy(SET CMP0069 NEW)
endif()

if (POLICY CMP0077)
   cmake_policy(SET CMP0077 NEW)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(ORBIT_BUILD_DOCS "Build the Orbit documentation" ON)

if (ORBIT_BUILD_DOCS)
   set(DOXYGEN_SKIP_DOT TRUE)
   find_package(Doxygen)
endif()

if (DOXYGEN_FOUND AND ORBIT_BUILD_DOCS)
   add_subdirectory(docs)
endif()
