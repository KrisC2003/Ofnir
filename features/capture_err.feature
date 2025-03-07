Feature: Capture errors and show in case of failures

    As a tester,
    to simplify failure diagnostics
    I want to:
        - capture output only when failures/errors occur
        - all output is displayed when capture is disabled

    Scenario: Test Setup
        Given a new directory
        and 