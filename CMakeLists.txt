project(lodTool)
cmake_minimum_required(VERSION 3.0.2)

# VCMI version
set(${project}_VERSION_MAJOR 0)
set(${project}_VERSION_MINOR 1)
set(${project}_VERSION_PATCH 0)

option(ENABLE_INTERFACE "Enable compilation of Interface mode" OFF)

set(CMAKE_AUTOMOC ON)

find_package(ZLIB REQUIRED)
find_package(Boost COMPONENTS filesystem program_options REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )

if(ENABLE_INTERFACE)
	# Widgets finds its own dependencies (QtGui and QtCore).
	find_package(Qt5Widgets REQUIRED)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR NOT WIN32) #so far all *nix compilers support such parameters
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CLANG_SPECIFIC_FLAGS "-Wno-mismatched-tags -Wno-unknown-warning-option")
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Wpointer-arith -Wno-strict-aliasing -Wno-switch -Wno-sign-compare -Wno-unused-local-typedefs  -Wno-unused-parameter -Wuninitialized -Wno-overloaded-virtual -Wno-type-limits -Wno-unknown-pragmas -Wno-reorder ${CLANG_SPECIFIC_FLAGS}")

	if(UNIX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
	endif()

endif()
if(ENABLE_INTERFACE)
	add_subdirectory(launcher)
endif()

set(${project}_SRCS
	main.cxx
)

add_executable(lodTool ${${project}_SRCS})
target_link_libraries(lodTool ${Boost_LIBRARIES})
