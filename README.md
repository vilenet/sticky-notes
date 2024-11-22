# Sticky Notes Program — Version 1

- [Actual Alpha Version 2](https://github.com/vilenet/sticky-notes2.git)

### Overview
This is the **first version** of the Sticky Notes program, which removes the standard **Win32 title bar** and implements a fully custom title bar using **FLTK**. This version does not rely on the native WinAPI for window controls, allowing for complete customization of the window's appearance and behavior.

### Key Features
- **No Win32 title bar**: The default title bar is completely removed, giving full control over the window design.
- Implements a **custom title bar** with custom buttons (minimize, maximize, close).
- Provides a **clean, minimalistic interface** optimized for note-taking, without distractions.
- Fully customizable **menu and window behavior** using FLTK.
- Enhanced control over window interactions, such as dragging, resizing, and closing.

### Design Philosophy
The goal of this version is to create a **highly customizable and modern-looking interface**, free from the constraints of the default Win32 window style. By using FLTK to draw the window elements, we can:

- Design a **unique title bar** with custom buttons and menus.
- Allow users to define their own window behavior, such as dragging and resizing.
- Maximize the text note area by removing unnecessary system elements.
- Provide a more modern and polished user experience.

### Key Differences from Version 2
- **Custom FLTK-based title bar**: Unlike Version 2, which retains the standard Win32 title bar with extended functionality, this version replaces it entirely with a custom-drawn title bar.
- **No direct use of WinAPI**: All window management, rendering, and behavior are handled through FLTK, without relying on WinAPI functions.
- **Custom buttons and menus**: The window buttons (minimize, maximize, close) are fully customizable and can be extended with additional functionality.

### Usage
1. Launch the program to open the Sticky Notes window with a custom title bar.
2. Use the custom buttons for minimizing, maximizing, or closing the window.
3. Drag the window by clicking and dragging on the custom title bar.
4. Maximize your note-taking area with a minimalistic and distraction-free interface.

### Future Enhancements
- Support for **custom themes** and colors for the title bar and buttons.
- Additional window behaviors, such as snapping to screen edges.
- Integration of **hotkeys** for enhanced window control and note management.
- Saving and loading notes directly within the application.

### Technologies Used
- **C++**
- **FLTK (Fast Light Toolkit)** — used for custom window rendering and UI components.

### License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

