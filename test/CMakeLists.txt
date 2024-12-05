enable_testing()
add_executable(RefCounterMain TestRefCounter.cpp)
target_link_libraries(RefCounterMain Core gtest gtest_main)


add_executable(ExprMain ExprUnitTest.cpp)
target_link_libraries(ExprMain Core gtest gtest_main)

add_test(NAME RefCounterTest COMMAND RefCounterMain)
add_test(NAME ExprTest COMMAND ExprMain)