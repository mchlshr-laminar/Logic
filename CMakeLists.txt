cmake_minimum_required(VERSION 3.12)
project(LogicVerifier)

add_subdirectory("${PROJECT_SOURCE_DIR}/Justifications")
add_subdirectory("${PROJECT_SOURCE_DIR}/Proof")
add_subdirectory("${PROJECT_SOURCE_DIR}/Statements")
	
add_executable(logicVerifier "${CMAKE_CURRENT_SOURCE_DIR}/ProofMain.cpp")
target_link_libraries(logicVerifier Justifications Proof Statements)
target_include_directories(logicVerifier PUBLIC 
	"${PROJECT_SOURCE_DIR}/Justifications" 
	"${PROJECT_SOURCE_DIR}/Proof"
	"${PROJECT_SOURCE_DIR}/Statements"
	)

install(TARGETS logicVerifier DESTINATION "${PROJECT_SOURCE_DIR}/bin")