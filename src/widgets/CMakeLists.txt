# put in cpp/hpp/ui files here inside widgets folder
# add_subdirectory if nesting another folder into widget

target_sources(${PROJECT_NAME} PRIVATE 
	infowindow.cpp
	screencapturewidget.cpp
	#translationoverlaywidget.cpp
	trayicon.cpp

	infowindow.h
	screencapturewidget.h
	#translationoverlaywidget.h
	trayicon.h

	infowindow.ui
)