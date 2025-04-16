# put in cpp/hpp/ui files here inside widgets folder
# add_subdirectory if nesting another folder into widget

target_sources(${PROJECT_NAME} PRIVATE 
	infowindow.cpp
	ocrtranslate.cpp
	screencapturewidget.cpp

	infowindow.h
	ocrtranslate.h
	screencapturewidget.h

	infowindow.ui
 )