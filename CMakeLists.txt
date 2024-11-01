cmake_minimum_required(VERSION 3.28)
project(capybara)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(MSVC)
    add_compile_options(/std:c++20 /experimental:module)
else()
    add_compile_options(-std=c++20)
endif()


find_package(LLVM CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

llvm_map_components_to_libnames(LLVM_LIBRARIES support core irreader object linker)
message(STATUS "LLVM_LIBRARIES: ${LLVM_LIBRARIES}")

include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(3rd)
add_subdirectory(main)

add_subdirectory(test)


