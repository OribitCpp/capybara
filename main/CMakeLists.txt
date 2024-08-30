set(TARGET_NAME "Main")
file(GLOB MAIN_H "*.h")
file(GLOB MAIN_CPP "*.cpp")

add_library(${TARGET_NAME} ${MAIN_H} ${MAIN_CPP})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
