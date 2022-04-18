# CMakeList.txt: LibuvTest 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.14)
project ("examples")

message(STATUS "include ${CMAKE_SOURCE_DIR}/include")
include_directories(${CMAKE_SOURCE_DIR}/include)

message(STATUS "link ${PROJECT_SOURCE_DIR}/lib")
link_directories(${PROJECT_SOURCE_DIR}/lib)


# 将源代码添加到此项目的可执行文件。
set(EXE_LIST file hello idle)

foreach(X IN LISTS EXE_LIST)
	add_executable (${X} ${X}/main.c)
	message(STATUS "linked ${X}")
	target_link_libraries(${X} uv.lib)  # 连接libuv静态库
endforeach()
