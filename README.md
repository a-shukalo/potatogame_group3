# Brotato MVP Game - Setup Instructions

A Brotato-inspired top-down survival shooter game built with C++ and SDL2.

## 🎮 Game Features

- **Mouse Aiming**: Gun follows your mouse cursor
- **Experience System**: Collect green experience orbs dropped by enemies
- **Character Progression**: Level up and gain stats automatically
- **Wave-Based Survival**: Waves start at 20 seconds and increase by 5 seconds each wave (max 60s)
- **Brotato-Style UI**: Health bar, level display, materials counter, and countdown timer
- **Materials System**: Earn materials by killing enemies
- **Character Stats**: Speed, damage, pickup range, armor, health regen, and more
- **Main Menu System**: Navigate with keyboard/mouse, ESC to pause during gameplay
- **Shop System**: Wave-end purchasing interface with material-based economy

## 🛠️ Prerequisites

### Required Software
1. **Visual Studio 2019/2022** (with C++ development tools)
2. **Git** for version control
3. **vcpkg** package manager
4. **CMake** (3.16 or higher)

### Windows Environment
- Windows 10/11
- PowerShell (for running commands)

## 📦 Installation Steps

### Step 1: Install vcpkg Package Manager

1. **Clone vcpkg** (if you don't have it):
   ```powershell
   cd C:\
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **Integrate vcpkg with Visual Studio**:
   ```powershell
   .\vcpkg integrate install
   ```

### Step 2: Install Required Libraries

```powershell
cd C:\vcpkg
.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-image:x64-windows
```

### Step 3: Clone/Download the Game

1. **If using Git**:
   ```powershell
   cd C:\Users\[YourUsername]\Documents\GitHub
   git clone [your-repository-url] potatogame
   ```

2. **Or manually**: Copy the game folder to your desired location

### Step 4: Verify Project Structure

Your project should look like this:
```
potatogame/
├── CMakeLists.txt
├── README.md
├── vcpkg.json
├── build-macos.sh
├── src/
│   ├── main.cpp
│   ├── Game.cpp
│   ├── Game.h
│   ├── Player.cpp
│   ├── Player.h
│   ├── Enemy.cpp
│   ├── Enemy.h
│   ├── SlimeEnemy.cpp
│   ├── SlimeEnemy.h
│   ├── PebblinEnemy.cpp
│   ├── PebblinEnemy.h
│   ├── Bullet.cpp
│   ├── Bullet.h
│   ├── Vector2.cpp
│   ├── Vector2.h
│   ├── ExperienceOrb.cpp
│   ├── ExperienceOrb.h
│   ├── Material.cpp
│   ├── Material.h
│   ├── Weapon.cpp
│   ├── Weapon.h
│   ├── Shop.cpp
│   ├── Shop.h
│   ├── Menu.cpp
│   └── Menu.h
├── assets/
│   ├── fonts/
│   │   └── default.ttf
│   ├── character/
│   ├── enemies/
│   ├── weapons/
│   └── ui/
└── monsters/
    └── landmonster/
        └── Transparent PNG/
            ├── idle/
            │   ├── frame-1.png
            │   └── frame-2.png
            └── got hit/
                └── frame.png
```

### Step 5: Build the Game

1. **Open PowerShell** in the project directory
2. **Create build directory**:
   ```powershell
   mkdir build
   cd build
   ```

3. **Configure with CMake**:
   ```powershell
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

4. **Build the project**:
   ```powershell
   cmake --build .
   ```

### Step 6: Run the Game

```powershell
.\Debug\BrotatoGame.exe
```

## 🎮 How to Play

### Controls
- **Movement**: WASD or Arrow Keys
- **Aiming**: Move your mouse cursor
- **Shooting**: Spacebar (hold for continuous fire)
- **Menu Navigation**: Arrow Keys + Enter, or mouse click
- **Pause Game**: ESC key (during gameplay)
- **Shop**: Automatically opens at end of each wave

### Gameplay
1. **Start from Menu**: Use main menu to begin new game
2. **Survive the Waves**: Each wave lasts 20-60 seconds
3. **Kill Enemies**: Shoot the blue monsters that spawn from screen edges
4. **Collect Experience**: Walk over green glowing orbs to gain XP
5. **Level Up**: Automatically gain stats when you have enough XP
6. **Earn Materials**: Get materials for each enemy killed
7. **Shop Between Waves**: Purchase weapons and upgrades with materials
8. **Pause Anytime**: Press ESC to pause and access menu options

### UI Elements
- **Top-Left Red Bar**: Your health (X / Y format)
- **Top-Left Gray Box**: Your current level (LV.X)
- **Top-Left Green Circle**: Materials collected
- **Center Top**: Current wave number and countdown timer
- **Bottom Green Bar**: Experience progress to next level

## 🔧 Troubleshooting

### Common Issues

1. **"SDL2 not found" Error**:
   - Make sure vcpkg is properly installed and integrated
   - Verify SDL2 packages are installed: `.\vcpkg list | grep sdl2`

2. **Monster Sprites Not Loading**:
   - Check that the `monsters/` folder is in the same directory as the executable
   - The build process should automatically copy monster assets

3. **Game Won't Start**:
   - Make sure you're running from the `build` directory
   - Check that all dependencies are installed
   - Verify Visual Studio C++ runtime is installed

4. **Build Errors**:
   - Ensure CMake is version 3.16 or higher
   - Make sure vcpkg toolchain path is correct
   - Try cleaning and rebuilding: `cmake --build . --clean-first`

### Performance Issues
- The game runs at ~60 FPS
- If experiencing lag, check Task Manager for other running processes
- Ensure graphics drivers are up to date

## 🚀 Development Notes

### Code Structure
- **Game.cpp/h**: Main game loop, rendering, and state management
- **Player.cpp/h**: Player character, stats, and progression system
- **Enemy.cpp/h**: Enemy AI, sprite animation, and behavior
- **Bullet.cpp/h**: Projectile physics and collision detection
- **ExperienceOrb.cpp/h**: Experience collection system
- **Vector2.cpp/h**: 2D vector math utilities

### Adding Features
- Character stats are defined in `PlayerStats` struct
- Wave timing can be adjusted in `Game.cpp` (waveDuration variable)
- New enemy types can be added by extending the Enemy class
- UI elements are rendered in the `renderUI()` method

### Graphics System
- Uses custom bitmap font rendering (no external font dependencies)
- Sprite animations for enemies
- Particle-like effects for experience orbs
- Simple SDL2 rectangle-based UI elements

## 📋 System Requirements

### Minimum Requirements
- **OS**: Windows 10/11 or macOS 12.0+
- **CPU**: Any modern CPU (game is not CPU-intensive)
- **RAM**: 100MB available memory
- **Graphics**: Any GPU with basic 2D acceleration
- **Storage**: ~50MB for game files and dependencies

### Recommended
- **Resolution**: 1024x768 or higher (works great on Retina displays)
- **Mouse**: For precise aiming and menu navigation
- **Audio**: Speakers/headphones (if audio is added later)

### Platform-Specific
- **Windows**: Visual Studio 2019/2022 with C++ tools
- **macOS**: Xcode Command Line Tools, works on both Intel and Apple Silicon

## 📞 Support

If you encounter issues:
1. Check this README first
2. Verify all prerequisites are installed
3. Make sure vcpkg packages are up to date
4. Try rebuilding the project from scratch
5. Check Windows Event Viewer for detailed error messages

## 🎯 Quick Start Checklist

### Windows
- [ ] Visual Studio installed with C++ tools
- [ ] vcpkg installed and integrated
- [ ] SDL2 and SDL2_image installed via vcpkg
- [ ] Project files downloaded/cloned
- [ ] Built successfully with CMake
- [ ] Game runs and displays UI correctly

### macOS
- [ ] Xcode Command Line Tools installed
- [ ] Homebrew, CMake, and Ninja installed
- [ ] vcpkg cloned and bootstrapped
- [ ] Project files downloaded/cloned
- [ ] Built successfully with `./build-macos.sh`
- [ ] Game runs and displays UI correctly

### Both Platforms
- [ ] Monster sprite assets in correct location
- [ ] Main menu appears on startup
- [ ] Menu navigation works (keyboard/mouse)
- [ ] ESC key pauses game correctly
- [ ] Shop opens at end of waves

Once all items are checked, you should be able to enjoy the game! 🎮

---

**Version**: 1.1  
**Last Updated**: December 2024  
**Compatible**: Windows 10/11 (Visual Studio 2019/2022), macOS 12.0+ (Intel & Apple Silicon)

### Recent Updates (v1.1)
- ✅ **Main Menu System**: Full navigation with keyboard/mouse support
- ✅ **macOS Support**: Native builds for both Intel and Apple Silicon
- ✅ **Enhanced Controls**: ESC pause functionality, improved mouse handling
- ✅ **Shop System**: Wave-end purchasing with materials
- ✅ **Multiple Enemy Types**: Slime (ranged) and Pebblin (melee) enemies
- ✅ **Cross-Platform Assets**: TTF font support with fallback rendering

---

## 🍎 macOS Setup Instructions

### Prerequisites

1. **Xcode Command Line Tools** (for compiler and build tools)
2. **Homebrew** (package manager)
3. **Git** (for version control)

### Step 1: Install Required Tools

```bash
# Install Homebrew (if you don't have it)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install build tools
brew install cmake ninja
```

### Step 2: Install vcpkg Package Manager

```bash
# Clone vcpkg to your home directory
git clone https://github.com/microsoft/vcpkg.git "$HOME/vcpkg"

# Bootstrap vcpkg
"$HOME/vcpkg"/bootstrap-vcpkg.sh
```

**Note**: You do NOT need to install SDL2 manually — the project uses manifest mode (`vcpkg.json`) which automatically resolves dependencies during build.

**Optional**: Set default triplet for convenience:
```bash
# Add to your shell profile (~/.zshrc or ~/.bash_profile)

# On Apple Silicon (M1/M2/M3)
export VCPKG_DEFAULT_TRIPLET=arm64-osx

# On Intel Mac
# export VCPKG_DEFAULT_TRIPLET=x64-osx
```

### Step 3: Clone/Download the Game

```bash
# Clone the repository
git clone [your-repository-url] potatogame_group3
cd potatogame_group3
```

### Step 4: Build the Game (Recommended Method)

Use the provided build script for easy compilation:

```bash
# Make the script executable
chmod +x ./build-macos.sh

# Build for Debug (default)
./build-macos.sh

# Build for Release (optimized)
./build-macos.sh --config Release
```

**Advanced Options:**
```bash
# Universal build (both arm64 and x86_64)
./build-macos.sh --arch universal --config Release

# Custom vcpkg location
./build-macos.sh --vcpkg-root "/path/to/your/vcpkg"
```

### Step 5: Build with Raw CMake (Alternative)

If you prefer manual CMake setup:

**For Apple Silicon (M1/M2/M3):**
```bash
cmake -G Ninja -S . -B build/macos-arm64-ninja \
  -DCMAKE_TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build/macos-arm64-ninja
```

**For Intel Mac:**
```bash
cmake -G Ninja -S . -B build/macos-x64-ninja \
  -DCMAKE_TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
  -DCMAKE_OSX_ARCHITECTURES=x86_64 \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build/macos-x64-ninja
```

### Step 6: Run the Game

```bash
# Navigate to the build directory
cd build/macos-arm64-ninja  # or macos-x64-ninja for Intel

# Run the game
./BrotatoGame
```

Notes:
- Uses dynamic libraries from vcpkg by default — simplest for development.
- `find_package(SDL2*_CONFIG REQUIRED)` already works with vcpkg toolchain.
- If later you need a distributable `.app` or advanced universal packaging, we can extend the build.