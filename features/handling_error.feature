Feature: Performance and Error Handling
As a user
I want the app to be responsive and handle errors gracefully
So that I can have a smooth experience while using it

Scenario: Invalid file format error
Given the user uploads a file that is not an image
When the app attempts to process the file
Then an error message should appear stating "Invalid file format. Please upload an image." and return the user to home page

Scenario: App handles no internet connection
Given the user has extracted text
When the user attempts to translate without an internet connection
Then the app should display an error message "No internet. Please check your internet connection and try again."

Scenario: Translation fails due to API issues
Given the user has extracted text and requested translation
When the translation API is down
Then the app should display an error message "Translation service is currently unavailable."

Scenario: Translation fails due to unrecognizable characters
Given the user has extracted text and requested translation
When the provided screenshot unregonizable hand writing/ non-supportive language 
Then the app should display an error message "We cannot recognize the characters at the moment, please try again."
