link_libraries(glfw)

include_directories("${Orbit_SOURCE_DIR}/deps" "${Orbit_SOURCE_DIR}/include")

if (MATH_LIBRARY)
    link_libraries("${MATH_LIBRARY}")
endif()

# Workaround for the MS CRT deprecating parts of the standard library
if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (WIN32)
    set(ICON glfw.rc)
elseif(APPLE)
    set(ICON glfw.icns)
endif()

set(GLAD_GL "${Orbit_SOURCE_DIR}/deps/glad/gl.h")
set(GLAD_GLES2 "${Orbit_SOURCE_DIR}/deps/glad/gles2.h")
set(GETOPT "${Orbit_SOURCE_DIR}/deps/getopt.h"
           "${Orbit_SOURCE_DIR}/deps/getopt.c")
set(TINYCTHREAD "${Orbit_SOURCE_DIR}/deps/tinycthread.h"
                "${Orbit_SOURCE_DIR}/deps/tinycthread.c")
set(SHADER "${Orbit_SOURCE_DIR}/deps/shader.h"
           "${Orbit_SOURCE_DIR}/deps/shader.cpp")
set(STB_IMG "${Orbit_SOURCE_DIR}/deps/stb_image.h")

# Add GLAD
add_library(glad "../src/glad.c")

add_executable(qtwindow WIN32 MACOSX_BUNDLE qtwindow.cpp ${GLAD_GL})

# Target link library for custom app
target_link_libraries(qtwindow PRIVATE glad ${CMAKE_DL_LIBS})

set(GUI_ONLY_BINARIES qtwindow)

if (MSVC)
    # Tell MSVC to use main instead of WinMain
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    # Tell Clang using MS CRT to use main instead of WinMain
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES
        LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()

if (APPLE)
    set_target_properties(qtwindow PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Qtwindow")
endif()
