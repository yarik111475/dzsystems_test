cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME dzsystems_test)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    set(BOOST_ROOT "/usr")
    set(BOOST_INCLUDEDIR "/usr/include")
    set(BOOST_LIBRARYDIR "/usr/lib")
endif()

set(Boost_DEBUG 1)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.76.0 EXACT COMPONENTS
    system
    filesystem
    program_options
)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
)
