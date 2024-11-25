cmake_minimum_required(VERSION 3.10)
project(capybara)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(LLVM CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

llvm_map_components_to_libnames(LLVM_LIBRARIES support core irreader object linker	mcjit
)
message(STATUS "LLVM_LIBRARIES: ${LLVM_LIBRARIES}")

include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(3rd)
add_subdirectory(main)

add_subdirectory(test)