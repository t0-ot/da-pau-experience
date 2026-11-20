# Da-Pau Launching Experience 🎮
> An Unbeaten Launching Experience

**Da-Pau** is a modular multi-game application written in C, built around a shared game-engine architecture. What started as a simple terminal-based Snake game and a way to practice C programming has gradually evolved into a unified application where multiple games can be launched and played through a single interface.

The project serves both as a fun side project and an ongoing exploration of game development, software architecture, the C programming language, and our schools final machine problem output.

## 🎮 Overview

Rather than developing each game as an entirely separate application, Da-Pau brings them together under one launcher. Games share common systems and infrastructure, allowing individual game modules to focus on their own gameplay while relying on the same underlying engine components.

The goal is to create a cohesive gaming experience while keeping the architecture modular, understandable, and extensible.

## ✨ Features

### Application
- Unified launcher for accessing multiple games.
- Modular game architecture, allowing individual games to be registered and launched independently.
- Shared game session and lifecycle management.
- Consistent application interface across games.

### Engine Architecture
- **Rendering System** – Shared rendering interfaces powered by Raylib.
- **Asset Management** – Centralized management of game assets, including texture resources and asset handles.
- **Input System** – Shared input handling and movement utilities.
- **Game Module System** – A common interface for initializing, updating, rendering, and cleaning up individual games.
- **Storage System** – Persistent storage utilities, including high-score management.
- **Animation System** – A developing animation subsystem for spritesheet-based animations.

### Games
- Multiple independently developed games integrated into one application.
- Each game maintains its own gameplay logic while utilizing shared engine functionality.

## 🛠️ Built With

- **C** – Primary programming language.
- **Raylib** – Graphics, rendering, and multimedia foundation.
- **Git** – Version control.
- **Cppcheck** – Static analysis and code quality checks.

## 🚀 Getting Started

Da-Pau is currently distributed as a source-code project. Precompiled binaries and an installer are not yet available.

To try the application, clone the repository, make sure the required build dependencies are available, and compile the project using its build configuration.

### Prerequisites
* **Git**, **GCC**, and **Make** (e.g., `build-essential` on Debian/Ubuntu, or **MinGW-w64** / **MSYS2** on Windows)
* **Linux users only:** X11 and OpenGL development headers (e.g., `libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev`) required to compile Raylib

### 1. Clone the Repository
Include `--recursive` so the Raylib submodule in `external/raylib` is downloaded:
```bash
git clone --recursive https://github.com
cd da-pau-experience
```

### 2. Build & Run

#### **Linux**
1. Compile the game (defaults to debug mode, or use `make release`):
   ```bash
   make
   ```
2. Run the executable:
   ```bash
   ./bin/Debug/Da-Pau-Launching-Experience
   ```
   *(If you built with `make release`, run `./bin/Release/Da-Pau-Launching-Experience` instead.)*

#### **Windows**
1. Compile the game using `mingw32-make` (Command Prompt/PowerShell) or `make` (MSYS2/Git Bash):
   ```cmd
   mingw32-make
   ```
   *(Use `mingw32-make release` to build without the background console window.)*
2. Run the executable:
   * **Command Prompt (`cmd`):**
     ```cmd
     bin\Debug\Da-Pau-Launching-Experience.exe
     ```
   * **PowerShell / Git Bash / MSYS2:**
     ```bash
     ./bin/Debug/Da-Pau-Launching-Experience.exe
     ```
   *(Your Makefile outputs to both `bin\Windows\Debug\` and `bin\Debug\`, so either path works.)*

## 🗺️ Roadmap

Da-Pau is an ongoing project, and there are still plenty of improvements planned.

- [ ] Complete application installer and distribution.
- [ ] Integrate sound effects and audio functionality.
- [ ] Expand the use of spritesheets and visual assets.
- [ ] Further develop the shared animation system.
- [ ] Add more games to the launcher.
- [ ] Continue improving and refining the shared engine architecture.

## 🤝 Contributing

Contributions, suggestions, and feedback are always welcome!

If you're interested in improving the project, feel free to open an issue or submit a pull request. Whether it's improving existing systems, adding a new game, or suggesting architectural improvements, We'd love to see what you come up with.

This project is also a learning experience for us, so constructive feedback and opportunities to learn are especially appreciated.

---

Made with C, curiosity, and probably an unreasonable amount of time spent thinking about software architecture. 😋
