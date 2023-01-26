cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(BOARD nucleo_f401re)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(TYPHOON CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(lib)
add_subdirectory(src)
