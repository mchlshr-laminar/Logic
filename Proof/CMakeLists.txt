add_library(Proof STATIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/Proof.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ProofReader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ProofRules.cpp"
	)
	
target_include_directories(Proof PUBLIC 
	"${PROJECT_SOURCE_DIR}/Justifications" 
	"${PROJECT_SOURCE_DIR}/Proof"
	"${PROJECT_SOURCE_DIR}/Statements"
	"${PROJECT_SOURCE_DIR}/rapidxml"
	)