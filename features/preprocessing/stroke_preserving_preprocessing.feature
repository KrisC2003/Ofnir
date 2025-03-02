Feature: Stroke-Preserving Preprocessing for OCR

    As a user working with a stroke based language,
    I want the preprocessing system to preserve the fine strokes and character details
    So the OCR can accurately recognize complex characters like kanji, hanzi and similar characters.

    Background:
        Given the OCR system has selected the stroke-preserving preprocessing pipeline

    Scenario: Apply stroke-preserving preprocessing filters in correct order
        When the preprocessing starts
        Then the system applies the following filters in order:
            | Step | Filter                          | Purpose                                     |
            | 1    | Grayscale Conversion            | Remove color, preserve strokes              |
            | 2    | Contrast Stretching             | Enhance stroke/background separation        |
            | 3    | Adaptive Thresholding           | Binarize while preserving fine details      |
            | 4    | Light Sharpening (Unsharp Mask) | Enhance edges without oversharpening        |
            | 5    | Bilateral Filtering (if noisy)  | Denoise while keeping edges intact          |
            | 6    | Deskewing (if skew detected)    | Straighten text without cutting off strokes |
        And re-runs confidence check after every filter until satisfactory

    Scenario: Reject stroke destructive filters
        When the preprocessing pipeline is chosen
        Then the system must avoid applying:
            | Filter                         | Reason                           |
            | Aggressive Smoothing           | May erase fine strokes           |
            | Global Thresholding            | May lose thin strokes            |
            | Excessive Sharpening           | May split strokes into artifacts |
            | Morphological Closing          | May merge distinct strokes       |

    Scenario: Post-preprocessing confidence check for stroke-preserving pipeline
        When the OCR runs after preprocessing
        Then the confidence level should exceed 70%
        And if confidence is still low, the user should be allowed to manually apply additional filters like:
            | Manual Filter Option    |
            | Deskewing               |
            | Background Removal      |
            | Additional Denoising    |