file(GLOB MAIN_H "*.h")
file(GLOB MAIN_CPP "*.cpp")

add_executable(${PROJECT_NAME} ${MAIN_H} ${MAIN_CPP} )

target_link_libraries(${PROJECT_NAME} PUBLIC Core)