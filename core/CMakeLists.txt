set(TARGET_NAME "Core")
file(GLOB CORE_H "*.h")
file(GLOB CORE_CPP "*.cpp")

add_library(${TARGET_NAME} ${CORE_H} ${CORE_CPP})

target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
