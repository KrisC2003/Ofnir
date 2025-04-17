# put in cpp/hpp/ui files here inside core folder
# add_subdirectory if nesting another folder into core

target_sources(${PROJECT_NAME} PRIVATE 
	imgpreprocessing.cpp
	ofnirdaemon.cpp

	imgpreprocessing.h
	ofnirdaemon.h
)