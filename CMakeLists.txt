cmake_minimum_required(VERSION 3.10)


set(CMAKE_C_COMPILER "gcc-10")
set(CMAKE_CXX_COMPILER "g++-10")
set(CMAKE_CXX_STANDARD 17)
SET(GCC_COVERAGE_COMPILE_FLAGS "-ffast-math")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

project(LearnedHashMap)

file(GLOB_RECURSE INCLUDE "${PROJECT_SOURCE_DIR}/include/*.h")

add_executable(main
    "src/main.cpp"
)

target_include_directories(main PUBLIC
    INCLUDE
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(main PUBLIC OpenMP::OpenMP_CXX)
endif()
