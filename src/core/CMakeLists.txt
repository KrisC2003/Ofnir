# put in cpp/hpp/ui files here inside core folder
# add_subdirectory if nesting another folder into core

target_sources(${PROJECT_NAME} PRIVATE 
	imgpreprocessing.cpp
	
	imgpreprocessing.h
)