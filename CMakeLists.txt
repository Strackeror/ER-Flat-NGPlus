cmake_minimum_required(VERSION 3.29)

add_subdirectory("libER")

project(better_ng_plus)
add_library(better_ng_plus SHARED better_ng_plus.cpp)

# Set iterator debug level to 0 for ELDEN RING ABI compatibility
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
target_compile_features(better_ng_plus PRIVATE cxx_std_20)
target_link_libraries(better_ng_plus PRIVATE libER_static)
