set(TARGET_NAME "FileManager")
file(GLOB FILE_MANAGER_H "*.ixx")
file(GLOB FILE_MANAGER_CPP "*.cpp")

add_library(${TARGET_NAME})

target_sources(${TARGET_NAME}
        PUBLIC
        FILE_SET CXX_MODULES FILES
        ${FILE_MANAGER_H}
        ${FILE_MANAGER_CPP}
)
target_link_libraries(${TARGET_NAME} PUBLIC ${LLVM_LIBRARIES} Core)
