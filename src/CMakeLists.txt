
#reads cmake folder for Find<Package>.cmake
#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
#list(APPEND CMAKE_PREFIX_PATH "C:/Libraries/tesseract-install")
#find_package(Tesseract REQUIRED)

list(APPEND CMAKE_PREFIX_PATH "C:/Libraries/Qt/6.9.0/msvc2022_64")
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(curl REQUIRED)
find_package(opencv REQUIRED)

add_executable(${PROJECT_NAME})

add_subdirectory(widgets)

target_sources(${PROJECT_NAME} 
    PRIVATE
    main.cpp 
	resource.qrc
)
# Link directories
target_include_directories(${PROJECT_NAME} 
    PUBLIC
    #${Tesseract_INCLUDE_DIRS} 
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/../>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/widgets>

)
# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PUBLIC
    #${Tesseract_LIBRARIES}
    ${OpenCV_LIBS}
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    CURL::libcurl
)

# Testing build
#add_executable(Test 
#    "src/widgets/screenCaptureWidget.cpp" 
#    "src/test.cpp" 
#)
#target_include_directories(Test PRIVATE
#   ${OpenCV_INCLUDE_DIRS}
#   ${CMAKE_SOURCE_DIR}/src
#   ${CMAKE_SOURCE_DIR}/include For what ever reason separating .hpp files with include dir messes with AUTOMOC
#)
#target_link_libraries(Test PRIVATE 
#    ${OpenCV_LIBS}
#    Qt6::Core
#    Qt6::Gui
#    Qt6::Widgets
#)