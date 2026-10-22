cmake_minimum_required(VERSION 4.3)
project(8inf342_tp2)

set(CMAKE_CXX_STANDARD 20)

find_package(X11 REQUIRED)

add_executable(8inf342_tp2 main.cpp)

target_link_libraries(8inf342_tp2 PRIVATE X11::X11)