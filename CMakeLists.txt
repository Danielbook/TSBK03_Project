cmake_minimum_required(VERSION 3.8)
set(APP_NAME "TSBK03_Project")
project(${APP_NAME})

set(CMAKE_CXX_STANDARD 11)

if (APPLE)
    message("Using CMake config for Mac")

    set(INCLUDE_DIRS common common/Mac)
    file(GLOB SOURCE_FILES main.c common/*.c common/Mac/MicroGlut.m)

    find_package(OpenGL)
    if (NOT ${OPENGL_FOUND})
        message(FATAL "OpenGL not found")
    endif ()
    set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
    set(EXTRA_LIBRARIES ${OPENGL_LIBRARIES})

    find_package(X11)
    if (NOT ${X11_FOUND})
        message(FATAL "X11 not found")
    endif ()
    set(INCLUDE_DIRS ${INCLUDE_DIRS} ${X11_INCLUDE_DIR})
    set(EXTRA_LIBRARIES ${X11_LIBRARIES})

    set(EXTRA_LIBRARIES "-framework CoreFoundation -framework CoreGraphics -framework Cocoa")
else ()
    message("Using CMake config for Linux")

    set(INCLUDE_DIRS common common/Linux)
    file(GLOB SOURCE_FILES main.c common/*.c common/Linux/MicroGlut.c)
    set(EXTRA_LIBRARIES ${OPENGL_gl_LIBRARY} ${EXTRA_LIBRARIES} X11 m)
endif ()

include_directories(${INCLUDE_DIRS})

#message(WARNING "All include dirs: ${INCLUDE_DIRS}")
#message(WARNING "All source files: ${SOURCE_FILES}")

add_executable(${APP_NAME} ${SOURCE_FILES})
target_link_libraries(${APP_NAME} ${OPENGL_gl_LIBRARY} ${EXTRA_LIBRARIES})
