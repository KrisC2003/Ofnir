Feature: Standard Text Preprocessing for OCR

    As a user processing clear, simple text (English, Spanish, etc.)
    I want the preprocessing to clean up the image without unnecessary complexity
    So that OCR works efficiently and accurately

    Background:
        Given the OCR system has selected the standard text preprocessing pipeline

    Scenario: Apply basic preprocessing filters in correct order
        When the preprocessing starts
        Then the system applies the following filters:
            | Step | Filter                  | Purpose             |
            | 1    | Grayscale Conversion    | Remove color data   |
            | 2    | Light Thresholding      | Simple binarization |
            | 3    | Light Denoising         | Remove minor noise  |

    Scenario: Skip stroke-preserving filters
        When the preprocessing pipeline is chosen
        Then the system must avoid applying:
            | Filter                       | Reason                           |
            | Adaptive Thresholding        | Unnecessary for clear text       |
            | Bilateral Filtering          | Adds unnecessary processing time |
            | Stroke-Specific Enhancement  | Not needed for Latin scripts     |

    Scenario: Confidence check after standard preprocessing
        When the OCR runs after preprocessing
        Then the confidence level should exceed 70%
        And if confidence is below 70%, the system may apply:
            | Additional Filter Option |
            | Stronger Denoising       |
            | Contrast Enhancement     |
            | Deskewing (if detected)  |