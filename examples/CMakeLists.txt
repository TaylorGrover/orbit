link_libraries(glfw)

# Ensure gdb debugging is enabled and all warnings are set
add_compile_options("-Wall" "-g" "-fPIC" "-DQT_NO_VERSION_TAGGING")

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

# Add GLAD
add_library(glad "${Orbit_SOURCE_DIR}/src/glad.c")

add_executable(main_window WIN32 MACOSX_BUNDLE main_window.cpp ${ICON} ${GLAD_GL} ${SHADER} ${CAMERA} ${INPUT} ${ENTITY} ${SPHERE} ${SPHERE_MANAGER})
add_executable(paletteGL WIN32 MACOSX_BUNDLE paletteGL.cpp ${ICON} ${GLAD_GL} ${SHADER} ${INPUT})
add_executable(perlin WIN32 MACOSX_BUNDLE perlin.cpp ${ICON} ${GLAD_GL} ${SHADER})

# Target link library for custom app
target_link_libraries(main_window PRIVATE glad ${CMAKE_DL_LIBS})
target_link_libraries(paletteGL PRIVATE glad ${CMAKE_DL_LIBS})
target_link_libraries(perlin PRIVATE glad ${CMAKE_DL_LIBS})

target_compile_options(main_window PRIVATE "-Wall" "-g" "-std=c++2a")
target_compile_options(paletteGL PRIVATE "-Wall" "-g" "-std=c++2a")
target_compile_options(perlin PRIVATE "-Wall" "-g" "-std=c++2a")

set(GUI_ONLY_BINARIES main_window perlin)

if (MSVC)
    # Tell MSVC to use main instead of WinMain
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    # Tell Clang using MS CRT to use main instead of WinMain
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES
        LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()

if (APPLE)
    set_target_properties(main_window PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Qtwindow")
endif()
