cmake_minimum_required (VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.80 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

project(server)
add_executable(server)

target_sources(server PRIVATE
	src/async.cc
)

target_include_directories(server PUBLIC
"${PROJECT_BINARY_DIR}")
target_link_libraries(server ${Boost_LIBRARIES})