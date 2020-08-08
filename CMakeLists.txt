cmake_minimum_required(VERSION 3.10)

project(LearnedHashMap)

set(CMAKE_CXX_COMPILER "g++-10")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp  -std=c++17 -ggdb")


add_executable(main
    "src/main.cpp"
)

target_link_libraries( main  
    ${OpenCV_LIBS}
)

target_include_directories(main PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
