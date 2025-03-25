#include "OcrTranslate.hpp"



int main() {
    SetConsoleOutputCP(CP_UTF8);  
    SetConsoleCP(CP_UTF8);


    std::string imagePath;
    std::cout << " Enter the image path: "; //you can type test,test1,test2.png to try out first, they are in the folder
    std::getline(std::cin, imagePath);

    cv::Mat img = cv::imread(imagePath);
    if (img.empty()) {
        std::cerr << "can't read the photo, Please check Path¡£\n";
        return 1;
    }
    //pre-processing
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);           // Grayscale
    cv::GaussianBlur(img, img, cv::Size(5, 5), 0);        // Denoising
    cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);  // Binarization
    cv::imwrite("processed.png", img);

  


    /* test with photos in projrct file
   Pix* image = pixRead(R"(test.png)");
   if (!image) {
       std::cerr << "can't read the photo, Please check Path¡£\n";
       return 1;
   }
   */

   //OCR API
    std::string ocrResult = performOCRWithGoogleVision("processed.png");
    
    std::wcout.imbue(std::locale("chs"));
    std::wcout << L"\n OCR result£º\n" << utf8ToWstring(ocrResult) << std::endl;

    // translation
    std::string translated = translateText(ocrResult, "en");
    std::wcout << L"\n translation result£º\n" << utf8ToWstring(translated) << std::endl;

    std::ofstream outFile("output.txt", std::ios::out | std::ios::binary);
    outFile << "OCR Result:\n" << ocrResult << "\n\n";
    outFile << "Translation Result:\n" << translated << std::endl;

    return 0;
}
