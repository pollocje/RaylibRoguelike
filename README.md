# Roguelike

A medieval turn-based roguelike built with [Raylib](https://www.raylib.com/) and C++.

## Requirements

- [CMake](https://cmake.org/download/) 3.15 or newer
- A C++ compiler:
  - **macOS:** Xcode Command Line Tools (`xcode-select --install`)
  - **Windows:** [Visual Studio](https://visualstudio.microsoft.com/) (with "Desktop development with C++" workload) or [MinGW](https://www.mingw-w64.org/)
  - **Linux:** `sudo apt install build-essential` (or equivalent)
- [Git](https://git-scm.com/)
- An internet connection (required on the first build to download Raylib automatically)

## Building and Running

```bash
cmake -B build
cmake --build build
```

- **macOS / Linux:** `./build/semGame`
- **Windows:** `build\semGame.exe`
