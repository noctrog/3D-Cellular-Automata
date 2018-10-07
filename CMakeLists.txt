cmake_minimum_required(VERSION 2.6)

project (3D_Cellular_Automata)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package( OpenGL )

set (CMAKE_DEBUG_POSTFIX "_d")

find_package(PkgConfig REQUIRED)
set(COMMON_LIBS X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL rt dl SDL2 Shader Rule App )

set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

add_library(Shader
	    src/Shader/Shader.cpp)
add_library(Rule
	    src/Rule/Rule.cpp)
add_library(App
	    src/App/App.cpp
	    src/App/gl3w.c)
add_library(World
	    src/world/world.cpp)
add_library(World3d
	    src/world3d/world3d.cpp)

target_link_libraries(App Shader Rule World World3d)

set(RUN_DIR ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX -std=c++14")

add_executable(3D_Cellular_Automata src/3D_Cellular_Automata.cpp)
set_property(TARGET 3D_Cellular_Automata PROPERTY DEBUG_POSTFIX _d)
target_link_libraries(3D_Cellular_Automata ${COMMON_LIBS})

include_directories(include)

ADD_CUSTOM_TARGET(debug ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${PROJECT_SOURCE_DIR})
ADD_CUSTOM_TARGET(release ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${PROJECT_SOURCE_DIR})
