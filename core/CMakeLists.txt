set(TARGET_NAME "Core")
file(GLOB_RECURSE CORE_IXX "*.ixx")
file(GLOB_RECURSE CORE_CPP "*.cpp")

add_library(${TARGET_NAME} ${CORE_IXX} ${CORE_CPP})

target_compile_features(${TARGET_NAME} INTERFACE cxx_std_20)
target_compile_options(${TARGET_NAME} INTERFACE -fmodules-ts)
target_link_libraries(${TARGET_NAME} PUBLIC ucrt spdlog::spdlog ${LLVM_LIBRARIES} )