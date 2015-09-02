if(ENABLE_INTERFACE)
	add_subdirectory(client)
	find_package(Qt5Widgets)
endif()

if(Qt5Widgets_FOUND)
	include_directories(${Qt5Widgets_INCLUDE_DIRS})
endif()

set(${project}_SRCS
	main.cxx
)

add_executable(homm3Tool ${${project}_SRCS})
target_link_libraries(homm3Tool ${ZLIB_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${Boost_LIBRARIES} ${Qt5Widgets_LIBRARIES})