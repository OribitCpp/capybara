set(TARGET_NAME "Core")
file(GLOB_RECURSE CORE_H "*.h")
file(GLOB_RECURSE CORE_CPP "*.cpp")

add_library(${TARGET_NAME} ${CORE_H} ${CORE_CPP})

target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog ${LLVM_LIBRARIES})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
