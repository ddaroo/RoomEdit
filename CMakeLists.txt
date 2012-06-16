project(RoomEdit)
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)

#set(CMAKE_BUILD_TYPE Release)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -Wclobbered -Wempty-body -Wignored-qualifiers -Wmissing-field-initializers -Wsign-compare -Wtype-limits -Wuninitialized")
endif()

if(APPLE)
	set(CMAKE_CXX_FLAGS "-Wall -Wempty-body -Wmissing-field-initializers -Wsign-compare")
endif()

add_subdirectory(editor)
