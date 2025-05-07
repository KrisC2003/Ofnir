# put in cpp/hpp/ui files here inside core folder
# add_subdirectory if nesting another folder into core

target_sources(${PROJECT_NAME} PRIVATE 
	ofnirdaemon.cpp
	imgpreprocessing.cpp
	ocrmanager.cpp

	ofnirdaemon.h
	imgpreprocessing.h
	ocrmanager.h
 "../widgets/filelistwidget.h" "../widgets/filelistwidget.cpp")