


# 创建测试可执行文件
enable_testing()
add_executable(RefCounterMain TestRefCounter.cpp)
target_link_libraries(RefCounterMain Core gtest gtest_main)

# 添加测试
add_test(NAME RefCounterTest COMMAND RefCounterMain)