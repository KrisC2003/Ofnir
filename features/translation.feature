Feature: Translating extracted Text
As a user
I want to translate extracted text into another language
So that I can understand foreign text from screenshots

Scenario: Translate extracted text into English
Given user sucessfully extracted text from the screenshot
When user select "English" as target language
And press "translate" button
Then The app should display the traslated text in English

Scenario: Translation fails due to API issues
Given user sucessfully extracted text from the screenshot
When the translation API is down or having internet issue
Then the app should display an error message "Translation service is currently unavailable, please try again."
