Feature: Adaptive Image Filtering for Improved OCR Accuracy

    As a user
    I want the OCR to automatically improve image quality
    so that I get the most accurate translations.
    
    Scenario: Detect and apply stroke based preprocessing pipeline
        Given the detected language is a stroke based language (e.g., Chinese, Japanese)
        When the system selects a preprocessing pipeline
        Then the system should follow the "stroke_preserving_preprocessing.feature" pipeline    

    Scenario: Detect and apply standard preprocessing pipeline
        Given the detected language is a standard text language (e.g., English)
        When the system selects a preprocessing pipeline
        Then the system should follow the "standard_text_preprocessing.feature" pipeline    

    Scenario: Basic image preprocessing for high confidence results
        Given I crop a portion of my screen with clear text
        When the OCR processes the image
        Then the system applies:
            | Filter               |
            | Greyscale Conversion |
            | Light thresholding   |
        And the OCR result should have a confidence level of atleast 70%

    Scenario: Image preprocessing for low confidence results
        Given I crop a portion of my screen with faint or blurry text
        When the OCR processes the image
        And the initial confidence level is below 70%
        Then the system analyzes the image quality based on detected issues
            | Detected Issue | Filter Applied       |
            | Noise          | Denoising            |
            | Low Contrast   | Contrast Enhancement |
            | Blurry Text    | Sharpening           |
        And the system re-runs OCR after filtering
        And the final OCR confidence should improve

    Scenario: Image is heavily distorted or unreadable/unfixable
        Given I crop a portion of my screen with severely unreadable text
        When the OCR system processes the image
        And all filtering steps fail to raise confidence level above 50%
        Then the system prompts me to confirm or adjust detected text manually
        And offers an advanced filter menu including:
            | Filter Options   |
            | Deskewing        |
            | Manual Threshold |
            | Inverted Text    |
        And the OCR allows me to retry processing after manual intervention