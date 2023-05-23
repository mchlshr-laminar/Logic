add_library(Statements STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/ProofStatement.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/StatementTree.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/SubProof.cpp"
	)
	
target_include_directories(Statements PUBLIC 
	"${PROJECT_SOURCE_DIR}/Justifications" 
	"${PROJECT_SOURCE_DIR}/Proof"
	"${PROJECT_SOURCE_DIR}/Statements"
	)