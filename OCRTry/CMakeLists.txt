cmake_minimum_required(VERSION 3.10)
project(OCRTranslator)

set(CMAKE_CXX_STANDARD 17)


add_executable(OCRTranslator
    main.cpp
    ocr.cpp
    translator.cpp
)


find_package(PkgConfig REQUIRED)
pkg_check_modules(Leptonica REQUIRED lept)
find_package(Tesseract REQUIRED)


target_include_directories(OCRTranslator PRIVATE
    ${Tesseract_INCLUDE_DIRS}
    ${Leptonica_INCLUDE_DIRS}
)


target_link_libraries(OCRTranslator PRIVATE
    ${Tesseract_LIBRARIES}
    ${Leptonica_LIBRARIES}
    curl
)
