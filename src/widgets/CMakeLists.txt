# put in cpp/hpp/ui files here inside widgets folder
# add_subdirectory if nesting another folder into widget

target_sources(${PROJECT_NAME} PRIVATE 
	infowindow.cpp
	screencapturewidget.cpp
	resultoverlay.cpp
	trayicon.cpp

	infowindow.h
	screencapturewidget.h
	resultoverlay.h
	trayicon.h

	infowindow.ui
)