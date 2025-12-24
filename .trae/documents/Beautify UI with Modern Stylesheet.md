I will enhance the UI with a modern, "Card-style" design using a polished color palette (Indigo/Slate theme) and improved spacing.

**Proposed Changes:**
1.  **Update `MainWindow.cpp`**:
    -   **Refine Stylesheet**:
        -   **Global**: Switch to a cleaner Light Gray background (`#F3F4F6`) with Dark Gray text (`#374151`).
        -   **Cards**: Style `QGroupBox` as white cards with subtle borders and shadow effects (via styling).
        -   **Typography**: Use `Segoe UI` / `Microsoft YaHei` with better padding and font weights.
        -   **Inputs**: Modernize `QLineEdit` and `QComboBox` with softer borders (`#D1D5DB`) and Indigo focus states.
        -   **Buttons**: Increase button height and padding. Use an **Indigo** theme (`#6366F1`) for primary actions and **Emerald Green** (`#10B981`) for the "Save" button.
        -   **Log Area**: Convert the log view to a **Dark Terminal style** (`#1F2937` background) for better readability and contrast.
        -   **Progress Bar**: Cleaner, flat design.
    -   **Layout Tweaks**: Increase spacing between elements slightly to reduce clutter.

2.  **Compile**: Rebuild the project to apply changes.

No functional logic will be changed, only the visual presentation (`setupUi` function).