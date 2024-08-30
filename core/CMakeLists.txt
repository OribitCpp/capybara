set(TARGET_NAME "Core")
file(GLOB CORE_H "*.h")
file(GLOB CORE_CPP "*.cpp")

add_library(${TARGET_NAME} ${CORE_H} ${CORE_CPP})