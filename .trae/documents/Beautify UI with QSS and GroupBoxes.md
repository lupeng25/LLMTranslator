I will beautify the interface by applying a modern stylesheet (QSS) and reorganizing the layout with GroupBoxes.

**Proposed Changes:**
1.  **Modify `MainWindow.cpp`**:
    -   **Add Styling**: Apply a comprehensive CSS-like stylesheet to the main widget to style buttons, input fields, progress bars, and groups. The style will feature a modern color palette (likely a dark or clean light theme), rounded corners, and hover effects.
    -   **Layout Reorganization**:
        -   Wrap the **File Selection** area in a `QGroupBox`.
        -   Wrap the **Settings** area (Language, API, Model) in a `QGroupBox`.
    -   **Widget Tweaks**: Increase padding and spacing to make the UI breathe. Set minimum heights for buttons and inputs for better touch/click targets.

**Visual Style Goals:**
-   **Buttons**: Modern flat style with accent colors (e.g., blue for primary actions).
-   **Inputs**: Clean borders with focus highlighting.
-   **Progress Bar**: Smooth, modern look.
-   **Groups**: Distinct sections with clear headers.

No new files will be created; only `MainWindow.cpp` (and potentially `MainWindow.h` to add `QGroupBox` header) will be modified.