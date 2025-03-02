Feature: OCR Text Extraction from Screenshot
  As a user
  I want to extract text from a screenshot
  So that I can translate and understand foreign content

  Scenario: Successfully extract text from a valid screenshot
  Given the user has captured a screenshot
  When the user uploads the screenshot to the app
  Then the app should extract and display the recognized text

  Scenario: OCR fails due to low-quality image
  Given the user uploads a blurry or distorted screenshot
  When the OCR engine processes the image
  Then the app should notify the user with a message "Text could not be recognized, try a clearer image."
