# put in cpp/hpp/ui files here inside widgets folder
# add_subdirectory if nesting another folder into widget

target_sources(${PROJECT_NAME} PRIVATE 
	infowindow.cpp
	ocrTranslate.cpp
	screenCaptureWidget.cpp

	infowindow.h
	ocrTranslate.h
	screenCaptureWidget.h

	infowindow.ui
)