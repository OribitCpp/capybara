cmake_minimum_required(VERSION 3.10)
project(capybara)

set(CMAKE_CXX_STANDARD 17)


find_package(LLVM CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(3rd)
add_subdirectory(main)