Feature: Dictionary-based Translation

    As a user
    I want the system to provide me with definitions
    So that I get rich translations with definitions and extra information

    Background: 
        Given the OCR system has successfully extracted text from a cropped image
        And the dictionary setting is enabled

    Scenario: Retrieve translation from primary dicionary API
        When the system requests translation for the extracted text
        Then the system queries the dictionary API for translations
        And the system displays the translated text
        And the system displays definitions and usage examples if available

    Scenario: Translate using offline dictionary when available
        Given that there is no internet connection
        And a dictionary file exists for the detected language
        When the system requests a translation
        Then the system uses the offline dictionary
        And the system displays the definitions and usage examples if available

    Scenario: Show dictionary metadata
        When the translation is displayed
        And user interacts with the words
        Then the system should show grammatical information 
        And the system should show example sentences if provided by the dictionary API

    Scenario: Cache dictionary translation results
        Given the same text is translated again
        Then the system retrieves the cached dictionary result
        And the system does not call the dictionary API again

    Scenario: Allow user to switch translation target language
        Given the text is translated to Japanese
        When the user switches to Korean
        Then the system re-queries the dictionary API for the new target language
        And the UI updates with the new translation

    Scenario: Allow user to toggle translated language visibility
        Given the text is already translated
        When the user clicks a button/hotkey
        Then it toggles between the translated text and previous text

    
