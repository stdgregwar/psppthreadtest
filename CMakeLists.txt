cmake_minimum_required(VERSION 3.5.0)

project(pthread-test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(main main.cpp)
target_link_libraries(main PRIVATE physfs pthread pspdebug pspsdk pspdisplay pspge)

create_pbp_file(
	TARGET main
	ICON_PATH NULL
	BACKGROUND_PATH NULL
	PREVIEW_PATH NULL
	TITLE pthread-test
	BUILD_PRX
	)
