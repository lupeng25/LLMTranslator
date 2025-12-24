I will configure the debugging environment for VS Code (Trae) on Windows.

**Proposed Changes:**

1.  **Create `.vscode` directory** (if not exists).
2.  **Create `.vscode/tasks.json`**:
    *   Define a task `CMake Build (Debug)` that runs `cmake --build ${workspaceFolder}/build --config Debug`.
    *   This ensures the code is recompiled in Debug mode before starting the debugger.
3.  **Create `.vscode/launch.json`**:
    *   Define a debug configuration using `cppvsdbg` (Visual Studio Windows Debugger).
    *   Set the program path to `${workspaceFolder}/build/Debug/LLMTranslator.exe`.
    *   Set `preLaunchTask` to `CMake Build (Debug)`.
4.  **Compile Debug Version**:
    *   I will execute the build command manually once to ensure the Debug binaries and necessary PDB files are generated and ready for use.

This setup will allow you to press **F5** (or use the Run/Debug tab) to build and start debugging the application immediately.