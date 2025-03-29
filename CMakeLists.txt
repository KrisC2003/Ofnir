cmake_minimum_required(VERSION 3.15...3.31)

set(OFNIR_MAJOR_VERSION "0")
set(OFNIR_MINOR_VERSION "1")
set(OFNIR_PATCH_VERSION "0")
set(OFNIR_VERSION ${OFNIR_MAJOR_VERSION}.${OFNIR_MINOR_VERSION}.${OFNIR_PATCH_VERSION})

project(Ofnir VERSION ${OFNIR_VERSION})

# Set C++ version
set(CMAKE_CXX_STANDARD 17)

#reads cmake folder for Find<Package>.cmake
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

# Tesseract
find_package(Tesseract REQUIRED)

# Curl
find_package(CURL REQUIRED)

# OpenCV
find_package(OpenCV REQUIRED)

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/ocrTranslate.cpp
)

# Link directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Tesseract_INCLUDE_DIRS} 
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${Tesseract_LIBRARIES}
    ${OpenCV_LIBS} 
    CURL::libcurl
)

