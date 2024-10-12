file(GLOB MAIN_H "*.h")
file(GLOB MAIN_CPP "*.cpp")

add_executable(${PROJECT_NAME} ${MAIN_H} ${MAIN_CPP} )

target_link_libraries(${PROJECT_NAME} PUBLIC  ${LLVM_LIBRARIES}  Core FileManager)


if(WIN32)
	target_link_libraries(${PROJECT_NAME} PUBLIC LLVMX86CodeGen LLVMX86AsmParser LLVMX86Desc LLVMX86Info LLVMX86Disassembler LLVMX86TargetMCA)
endif()