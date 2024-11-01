set(TARGET_NAME "Core")
file(GLOB_RECURSE CORE_IXX "*.ixx")
file(GLOB_RECURSE CORE_CPP "*.cpp")
add_library(${TARGET_NAME}    )

target_sources(${TARGET_NAME}
        PUBLIC
        FILE_SET CXX_MODULES FILES
        ${CORE_IXX}
        ${CORE_CPP}
)


target_link_libraries(${TARGET_NAME} PUBLIC ucrt spdlog::spdlog ${LLVM_LIBRARIES} )