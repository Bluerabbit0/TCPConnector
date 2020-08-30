 # CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set (This TCPConnector)
project($(This) C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Add source to this project's executable.
add_executable ( TCPConnector "TCPConnector.cpp")
# TODO: Add tests and install targets if needed.