# Custom `ls` Utility

## Project Overview
This project is a rigorous re-engineering of the standard Unix `ls` command from scratch. It explores the intricacies of the Linux file system, system calls, and terminal formatting. Starting from a basic directory lister, the utility was incrementally evolved to support advanced features like long listing, recursion, sorting, and colorized output, mirroring the professional `GNU ls` behavior.

## Features Implemented

### 1. File Metadata & System Calls
- **Long Listing (`-l`):** utilized `stat()` and `lstat()` to retrieve file permissions, ownership (resolving UIDs/GIDs via `getpwuid`/`getgrgid`), size, and modification timestamps.
- **Permission Formatting:** Custom logic to translate bitwise `st_mode` integers into human-readable `rwxr-xr-x` strings.

### 2. Advanced Terminal Formatting
- **Columnar Display:** Implemented the complex "down-then-across" algorithm.
- **Terminal Adaptation:** Uses `ioctl()` to detect window size and dynamically adjust column widths.
- **Horizontal Display (`-x`):** Added support for row-major listing.

### 3. Sorting & Recursion
- **Alphabetical Sort:** Integrated `qsort()` with custom comparators to sort directory entries.
- **Recursive Listing (`-R`):** Implemented logic to traverse nested directory structures, constructing paths dynamically to visit every subdirectory.

### 4. User Experience
- **Colorized Output:** Integrated ANSI escape codes to distinctively color files based on type (Blue for Directories, Green for Executables, Red for Archives, etc.).

## Technologies Used
- **Language:** C
- **System Calls:** `opendir`, `readdir`, `stat`, `lstat`, `ioctl`
- **Build Tool:** GNU Make
- **Version Control:** Git (Atomic commits per feature)
