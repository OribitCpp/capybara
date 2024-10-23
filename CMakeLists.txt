cmake_minimum_required(VERSION 3.27)
project(capybara)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

llvm_map_components_to_libnames(LLVM_LIBRARIES support core irreader object linker)
message(STATUS "LLVM_LIBRARIES: ${LLVM_LIBRARIES}")

include_directories(${LLVM_INCLUDE_DIRS})

if(MSVC)
    if (CMAKE_BUILD_TYPE EQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")  # 对于 Release 版本
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MDd")  # 对于 Debug 版本
    endif ()
endif()


add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(3rd)
add_subdirectory(main)

add_subdirectory(test)