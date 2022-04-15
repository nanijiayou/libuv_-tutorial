# CMakeList.txt: LibuvTest 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.14)
project ("HelloLibuv")

# add_subdirectory(src)

message(STATUS "include ${CMAKE_SOURCE_DIR}/include")
include_directories(${CMAKE_SOURCE_DIR}/include)

message(STATUS "link ${PROJECT_SOURCE_DIR}/lib")
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 将源代码添加到此项目的可执行文件。
add_executable (Hello hello.c)
add_executable (Idle idling.c)
add_executable (Fs fs.c)

# 连接libuv静态库
target_link_libraries(Hello uv.lib)
target_link_libraries(Idle uv.lib)
target_link_libraries(Fs uv.lib)
