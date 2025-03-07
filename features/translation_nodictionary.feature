Feature: Direct Translation without dictionary

    As a user
    I want the system to directly translate text without definitions
    So that I can quickly understand the meaning

    Background: 
        Given the OCR system has successfully extracted text from a cropped image
        And the dictionary setting is disabled

    Scenario: Translate the given text using the primary API
        When the system requests translation for the extracted text
        Then the system should query the primary dictionary API for translations
        And the system should display translated text as hoverable over the original text

    Scenario: Allow user to switch translation language after translation
        Given the text is already translated
        When the user clicks a button or a hotkey
