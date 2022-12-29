cmake_minimum_required(VERSION 3.23)
project(Robot-Network LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

add_executable(CoreNode core_node.cpp)
add_executable(ChildNode child_node.cpp)