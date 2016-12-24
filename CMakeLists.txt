cmake_minimum_required(VERSION 3.0)
project(render)
#set(CMAKE_VERBOSE_MAKEFILE on)

# Check for build type. Default: DEBUG
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -fPIC")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Svg_INCLUDE_DIRS})

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/chromosome.cpp
  )

set(LIBS ${LIBS}
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Svg_LIBRARIES})

add_executable(render ${SOURCE_FILES})
target_link_libraries(render ${LIBS})
