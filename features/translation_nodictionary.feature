Feature: Direct Translation without dictionary

    As a user
    I want the system to directly translate text without definitions
    So that I can quickly understand the meaning

    Background: 
        Given the OCR system has successfully extracted text from a cropped image
        And the dictionary setting is disabled

    Scenario: Translate using direct translation service
        When the system requests a translation
        Then the system uses a basic translation API or built-in translation
        And the system displays the translated text without definitions or clickable metadata

    Scenario: Translate using offline translation table when available
        Given the system is offline
        And an offline translation table exists for the detected language
        When the system requests a translation
        Then the system uses the offline translation
        And the system displays the translated text directly

    Scenario: Warn user if no translation available
        Given the system is offline
        And no offline translation table exists for the detected language
        When the system requests a translation
        Then the system shows a message "Translation unavailable"

    Scenario: Cache direct translation results
        Given the same text is translated again
        Then the system retrieves the cached result
        And the system does not call the translation service again

    Scenario: Allow user to toggle translated language visibility
        Given the text is already translated
        When the user clicks a button/hotkey
        Then it toggles between the translated text and previous text
