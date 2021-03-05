cmake_minimum_required(VERSION 3.17)
project(concurrency_examples)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MEMCHECK_FLAGS} -Wall -Wextra ${WERROR_CXX_FLAGS} -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MEMCHECK_FLAGS} -Wall -Wextra ${WERROR_C_FLAGS}")

add_executable(concurrency_examples main.cpp)