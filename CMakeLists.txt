cmake_minimum_required(VERSION 3.29)

project(better_ng_plus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(minhook
  GIT_REPOSITORY        https://github.com/TsudaKageyu/minhook.git
  GIT_TAG               f5485b8454544c2f034c78f8f127c1d03dea3636)

FetchContent_Declare(pattern16
  GIT_REPOSITORY        https://github.com/Dasaav-dsv/Pattern16.git
  GIT_TAG               728eac543cee7e7bf6fda445d7db06e3dc8a61d0
  CONFIGURE_COMMAND     ""
  BUILD_COMMAND         "")

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(minhook pattern16 tomlplusplus)

add_library(pattern16 INTERFACE)
target_include_directories(pattern16 INTERFACE ${pattern16_SOURCE_DIR}/include)

add_library(from STATIC deps/from/modutils.cpp deps/from/params.cpp)
target_include_directories(from PUBLIC deps/from)
target_link_libraries(from PUBLIC minhook pattern16)

add_library(better_ng_plus SHARED better_ng_plus.cpp)
target_compile_features(better_ng_plus PRIVATE cxx_std_20)
target_link_libraries(better_ng_plus PRIVATE tomlplusplus::tomlplusplus from)
