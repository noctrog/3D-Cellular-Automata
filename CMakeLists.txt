cmake_minimum_required(VERSION 2.6)

project (3D_Cellular_Automata)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package( OpenGL )

set (CMAKE_DEBUG_POSTFIX "_d")

if (UNIX)
    find_package(PkgConfig REQUIRED)
    set(COMMON_LIBS Shader App X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL rt dl SDL2)
endif()

set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

add_library(Shader
	    src/Shader/Shader.cpp)
add_library(App
	    src/App/App.cpp
	    src/App/gl3w.c)

set(RUN_DIR ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX -std=c++14")

add_executable(3D_Cellular_Automata src/3D_Cellular_Automata.cpp)
target_link_libraries(3D_Cellular_Automata ${COMMON_LIBS})

include_directories(include)
