# put in cpp/hpp/ui files here inside settings folder
# add_subdirectory if nesting another folder into settings

target_sources(${PROJECT_NAME} PRIVATE 
	globalHotkeyFilter.cpp

	globalHotkeyFilter.h
)