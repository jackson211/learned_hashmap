cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "gcc-10")
set(CMAKE_CXX_COMPILER "g++-10")
set(CMAKE_CXX_STANDARD 17)

project(LearnedHashMap)


# find_package(OpenMP REQUIRED)
# if (OPENMP_FOUND)
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
# endif ()


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


