set(TARGET_NAME "Searcher")
file(GLOB SEARCHER_H "*.h")
file(GLOB SEARCHER_CPP "*.cpp")

add_library(${TARGET_NAME} ${SEARCHER_H} ${SEARCHER_CPP})
target_link_libraries(${TARGET_NAME} PUBLIC ${LLVM_LIBRARIES} Core)
