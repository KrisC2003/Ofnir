Feature: Settings for Ofnir

    As a user,
    I want to be able to customize the app
    So that I can customize the app to match my workflow

    Background: 
        Given I am on the settings tab
        
    Scenario: Changing Hotkeys
        When I open "Hotkeys" tab        
        And I update a hotkey for a tool
        Then I should be able to use the new hotkey immediately
        And the new hotkey is saved for future sessions
    
    Scenario: Setting theme 
        When I open the "Appearance" tab
        And I select a theme
        Then the setting UI will update to match the theme
        And the theme should apply across the app

    Scenario: Importing Custom Dictionaries
        When I open the "Dictionary" tab 
        And I select the option to import a custom dictionary
        And I choose a file in a supported format (XML, TSV/CSV, JSON, etc.)
        Then I should see a confirmation message "Dictionary imported successfully"

    Scenario: Exporting Custom Dictionary
        When I open the "Dictionary" tab 
        And I select a dictionary to export
        And I confirm the export action
        Then the selected dictionary file should be downloaded

    Scenario: Exporting All Custom Dictionary
        When I open the "Dictionary" tab
        And I choose the option to export all dictionaries
        Then a combined file containing all dictionaries should be downloaded

    Scenario: Dictionary Popup Behavior
        When I open the "Dictionary" tab
        And I choose hide popup on cursor exit
        Then when I open definitions once the cursor exits the area it will remove the popup.

    Scenario: Disable preprocessing prompt behaviors