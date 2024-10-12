set(TARGET_NAME "Runtime")
file(GLOB RUNTIME_H "*.h")
file(GLOB RUNTIME_CPP "*.cpp")

add_library(${TARGET_NAME} ${RUNTIME_H} ${RUNTIME_CPP})
target_link_libraries(${TARGET_NAME} PUBLIC ${LLVM_LIBRARIES} Core)
