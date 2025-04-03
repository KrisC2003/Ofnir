# FindTesseract.cmake - Locate Tesseract OCR library
set(TESSERACT_DIR "C:/Libraries/tesseract")

FIND_PATH(TESSERACT_INCLUDE_DIR 
	NAMES tesseract/baseapi.h
	PATHS
	C:/Libraries/tesseract-install/include

)

find_library(TESSERACT_LIBRARY
    NAMES tesseract
    PATHS
    C:/Libraries/tesseract-install/lib
)


if(TESSERACT_INCLUDE_DIR AND TESSERACT_LIBRARY)
   set(TESSERACT_FOUND TRUE)
   set(TESSERACT_LIBRARIES ${TESSERACT_LIBRARY})
else()
   set(TESSERACT_FOUND FALSE)
endif()