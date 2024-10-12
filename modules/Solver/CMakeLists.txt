set(TARGET_NAME "Solver")
file(GLOB SOLVER_H "*.h")
file(GLOB  SOLVER_CPP "*.cpp")

add_library(${TARGET_NAME} ${ SOLVER_H} ${ SOLVER_CPP})
target_link_libraries(${TARGET_NAME} PUBLIC ${LLVM_LIBRARIES} Core)
