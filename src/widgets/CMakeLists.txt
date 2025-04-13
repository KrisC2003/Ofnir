# put in cpp/hpp/ui files here inside widgets folder
# add_subdirectory if nesting another folder into widget

target_sources(${PROJECT_NAME} PRIVATE 
	mainwindow.cpp
	ocrTranslate.cpp
	screenCaptureWidget.cpp

	mainwindow.h
	ocrTranslate.h
	screenCaptureWidget.h

	mainwindow.ui
)