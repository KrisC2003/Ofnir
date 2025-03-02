Feature: User Interface and Workflow
As a user
I want a seamless and intuitive interface
So that I can efficiently extract and translate text

Scenario: User uploads a screenshot
Given the app is open and in the foreground 
When the user capture and paste the screenshot
And press "confirm" button or press enter key on keyboard
Then the app should process the image and extract text
 
Scenario: User uploads a image from file
Given the app is open and in the foreground 
When the user upload an image by pressing the upload icon
And press "confirm" button or press enter key on keyboard
Then the app should process the image and extract text

Scenario: User translates extracted text with one click
Given the extraction is complete and the text is displayed
When the user clicks on "Translate" button
Then the translation should appear next to the original text

  
