set(TARGET_NAME "Memory")
file(GLOB EXPR_H "*.h")
file(GLOB EXPR_CPP "*.cpp")

add_library(${TARGET_NAME} ${EXPR_H} ${EXPR_CPP})
target_link_libraries(${TARGET_NAME} PUBLIC ${LLVM_LIBRARIES} Core)
