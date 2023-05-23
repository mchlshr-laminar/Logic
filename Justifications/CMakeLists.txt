add_library(Justifications STATIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/AggregateJustification.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/EquivalenceRules.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/InferenceRules.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Justification.cpp"
	)

target_include_directories(Justifications PUBLIC 
	"${PROJECT_SOURCE_DIR}/Justifications" 
	"${PROJECT_SOURCE_DIR}/Proof"
	"${PROJECT_SOURCE_DIR}/Statements"
	)