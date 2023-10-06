link_libraries(glfw)

include_directories("${GLFW_SOURCE_DIR}/deps")

add_compile_options("-g" "-Wall" "-std=c++2a")

if (MATH_LIBRARY)
    link_libraries("${MATH_LIBRARY}")
endif()

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h")
set(GLAD_VULKAN "${GLFW_SOURCE_DIR}/deps/glad/vulkan.h")
set(GETOPT "${GLFW_SOURCE_DIR}/deps/getopt.h"
           "${GLFW_SOURCE_DIR}/deps/getopt.c")
set(TINYCTHREAD "${GLFW_SOURCE_DIR}/deps/tinycthread.h"
                "${GLFW_SOURCE_DIR}/deps/tinycthread.c")

add_executable(test_glm WIN32 MACOSX_BUNDLE test_glm.cpp)
add_executable(test_input WIN32 MACOSX_BUNDLE test_input.cpp ${INPUT})

add_test(NAME GLMTest COMMAND test_glm)
add_test(NAME InputTest COMMAND test_input)
