cmake_minimum_required(VERSION 3.10)
project(LearnedHashMap LANGUAGES CXX)


set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "g++-9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -std=c++17 -ggdb")


############################################################
# Create a library
############################################################

#Generate the static library from the library sources
## add_library(lib SHARED
##     "src/entry.cpp"
## )

## target_include_directories(lib PUBLIC
##     ${PROJECT_SOURCE_DIR}/include
## )

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(main
    "src/main.cpp"
)

# link the new hello_library target with the hello_binary target
target_link_libraries( main
    PRIVATE
    #        lib
)

target_include_directories(main PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
