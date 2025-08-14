# Brotato Game Project - Technical Documentation

## Project Overview

This is a **Brotato-inspired top-down survival shooter** built with **C++ and SDL2**. The game features wave-based survival gameplay where players control a potato character fighting against waves of enemies while collecting experience and materials to upgrade their character.

### Core Game Loop
1. **Survive waves** of enemies (20-60 seconds each)
2. **Kill enemies** to earn experience and materials
3. **Level up** and gain stats automatically
4. **Shop between waves** to purchase weapons and upgrades
5. **Progress through increasingly difficult waves**

---

## Architecture Overview

### Core Dependencies
- **SDL2** - Main rendering and input framework
- **SDL2_image** - Image loading (PNG support)
- **SDL2_ttf** - TrueType font rendering with bitmap fallback
- **C++** standard library (vectors, memory management, etc.)

### Build System
- **CMake** build configuration
- **vcpkg** package management for dependencies
- **Cross-platform** support (Windows, macOS via build scripts)

---

## Source Code Structure

### Main Files

#### `main.cpp` (15 lines)
- **Purpose**: Entry point, initializes and runs the game
- **Key Functions**: 
  - Creates `Game` instance
  - Calls `init()` and `run()`
  - Basic error handling

#### `Game.h/cpp` (~800 lines)
**The heart of the application - manages all game systems**

**Key Responsibilities:**
- **SDL2 initialization** (video, image, TTF)
- **Main game loop** (60 FPS with 16ms delay)
- **Input handling** (keyboard/mouse)
- **Rendering pipeline** (all entities + UI)
- **Wave management** (timing, progression)
- **Shop system integration**
- **Collision detection** (bullets vs enemies, player vs enemies)
- **Entity management** (bullets, enemies, orbs, materials)

**Notable Features:**
- **Enemy spawn telegraphing** (2-second red X indicators)
- **Brotato-style material drop system** with bag overflow
- **Custom bitmap font rendering** with TTF fallback
- **Fullscreen 1920x1080 display**
- **Comprehensive UI system** (health bars, timers, material counters)

#### `Player.h/cpp` (~267 lines)
**Player character with Brotato-like stats system**

**Player Stats:**
```cpp
struct PlayerStats {
    int maxHealth;        // Base: 100
    float moveSpeed;      // Base: 200
    float pickupRange;    // Base: 50 (visible green circle)
    float attackSpeed;    // Base: 1.0f
    int damage;           // Base: 10
    float range;          // Base: 200
    int armor;            // Damage reduction
    float healthRegen;    // HP per second
    float dodgeChance;    // 0.0-1.0
    int luck;             // Affects drops
    int materials;        // Currency, starts with 50
}
```

**Key Systems:**
- **WASD movement** with screen boundary clamping
- **Mouse-aimed shooting** direction
- **Experience-based leveling** (Brotato formula: (Level+3)²)
- **Weapon inventory** (max 6 weapons, circular positioning)
- **Sprite rendering** (brick character with fallback circle)
- **Health regeneration** system

#### `Enemy.h/cpp` + Specialized Enemies
**Base enemy class with animation system**

**Enemy Types:**
1. **Base Enemy** - Simple movement toward player
2. **SlimeEnemy** - Ranged shooter with cooldown
3. **PebblinEnemy** - Another ranged variant

**Features:**
- **State system** (IDLE, HIT) with animation frames
- **Sprite loading** from assets/enemies/
- **Collision detection** with configurable radius
- **Wave-based spawning** with type probability

#### `Weapon.h/cpp` (~95 lines)
**Weapon system with multiple types and tiers**

**Weapon Types:**
- **PISTOL** - Basic single-shot weapon
- **SMG** - High rate of fire
- **MELEE_STICK** - Brick on stick melee weapon

**Weapon Tiers:** TIER_1 through TIER_4 (scaling damage/stats)

**Key Features:**
- **Circular positioning** around player (50px radius)
- **Mouse-directed aiming** (all weapons point at cursor)
- **Melee attack system** with weapon tip collision
- **Sprite rendering** with weapon textures
- **Player stat scaling** (damage, attack speed, etc.)

#### `Shop.h/cpp` (~102 lines)
**Between-wave upgrade system**

**Shop Features:**
- **4-item selection** per wave
- **Weapon purchases** with material cost
- **Item locking** system
- **Reroll functionality** (costs materials)
- **Tier progression** based on wave number
- **Mouse + keyboard input** support

#### `Bullet.h/cpp` (~40 lines)
**Projectile system with multiple types**

**Bullet Types:**
- **PISTOL** - Standard straight-line bullet
- **SMG** - Faster, smaller bullets
- **ENEMY_LOB** - Gravity-affected projectiles from enemies

**Physics:**
- **Linear movement** with range limits
- **Collision detection** with configurable radius
- **Enemy-owned vs player-owned** damage systems

#### `ExperienceOrb.h/cpp` (~27 lines)
**XP collection system**

**Features:**
- **Bobbing animation** (visual appeal)
- **Lifetime management** (orbs expire)
- **Pickup range** detection
- **Configurable XP values** based on wave

#### `Material.h/cpp` (~29 lines)
**Resource collection system**

**Features:**
- **Dual-purpose** (materials + experience)
- **Drop chance formula** (Brotato-style: 100% decreasing to 50%)
- **Map limit enforcement** (max 50, overflow goes to bag)
- **Bobbing animation** similar to XP orbs

#### `Vector2.h/cpp` (~27 lines)
**2D vector math utilities**

**Operations:**
- Addition, subtraction, scalar multiplication
- **Distance calculation**
- **Normalization**
- **Length calculation**

---

## Game Systems Deep Dive

### Wave System
```cpp
// Wave timing progression
waveDuration = 20.0f;  // Wave 1
waveDuration += 5.0f;  // +5 seconds per wave
// Capped at 60 seconds (Wave 9+)
```

**Wave Progression:**
- Wave 1: 20 seconds
- Wave 2: 25 seconds
- Wave 3: 30 seconds
- ...
- Wave 9+: 60 seconds (maximum)

### Enemy Spawning System
**Spawn Telegraph System:**
- **2-second warning** with flashing red X
- **Spawn type determination** based on wave:
  - Wave 1: 50% Slime, 50% Base
  - Wave 2+: 40% Slime, 40% Pebblin, 20% Base

**Spawn Rate Formula:**
```cpp
float spawnRate = 1.0f - (wave * 0.1f);
if (spawnRate < 0.2f) spawnRate = 0.2f;  // Minimum 0.2 seconds
```

### Material Drop System
**Brotato-inspired drop rates:**
```cpp
float baseChance = 1.0f;
float reduction = (wave - 1) * 0.015f;  // -1.5% per wave
float dropChance = baseChance - reduction;
if (dropChance < 0.5f) dropChance = 0.5f;  // Minimum 50%
```

### Experience System
**Leveling Formula (Brotato-style):**
```cpp
int getExperienceToNextLevel() const {
    int nextLevel = level + 1;
    return (nextLevel + 3) * (nextLevel + 3);
}
```

**Level Requirements:**
- Level 2: 25 XP
- Level 3: 36 XP  
- Level 4: 49 XP
- Level 5: 64 XP
- etc.

### Collision Detection
**Systems in place:**
1. **Player vs Enemy** - Direct damage to player
2. **Player Bullets vs Enemies** - Enemy destruction + XP/material drops
3. **Enemy Bullets vs Player** - Player damage with armor/dodge calculations
4. **Melee Weapons vs Enemies** - Tip-based collision during attack animation
5. **Player vs Pickups** - XP/material collection within pickup range

### UI Rendering System
**Custom Bitmap Font System:**
- **5x7 pixel font** for numbers and basic letters
- **TTF fallback** for more complex text
- **Color-coded elements:**
  - Red health bar with white text
  - Green materials circle counter
  - Green experience progress bar
  - White wave/timer display on dark background

---

## File Organization

### Asset Structure
```
assets/
├── character/
│   └── brick.png              # Player sprite
├── enemies/
│   ├── slime.png             # Slime enemy
│   ├── pebblin.png           # Pebblin enemy
│   └── mortorhead.png        # Additional enemy
├── weapons/
│   ├── pistol.png            # Various weapon sprites
│   ├── smg.png
│   ├── brickonstick.png
│   └── [many more weapon sprites]
├── ui/
│   ├── card_normal.png       # Shop UI elements
│   ├── coin.png
│   └── lock_unlocked.png
└── fonts/
    └── default.ttf           # TTF font with fallbacks
```

### Build Scripts
- **build-game.bat/ps1** - Windows build automation
- **build-macos.sh** - macOS build with vcpkg
- **quick-build.bat** - Fast iteration builds
- **debug-build.bat** - Debug configuration builds

---

## Technical Highlights

### Memory Management
- **RAII principles** with smart pointers (`std::unique_ptr`)
- **STL containers** for entity management
- **Automatic cleanup** in destructors
- **Resource management** for SDL textures

### Performance Optimizations
- **60 FPS target** with 16ms frame limiting
- **Efficient collision detection** using distance calculations
- **Entity cleanup** via `std::remove_if` patterns
- **Texture caching** for sprite rendering

### Error Handling
- **Graceful degradation** (bitmap fonts when TTF fails)
- **Asset loading fallbacks** (colored shapes when sprites fail)
- **SDL error reporting** with console output
- **Initialization validation** at startup

### Platform Support
- **Windows** (primary platform with Visual Studio)
- **macOS** (via build script with universal binary support)
- **Cross-platform SDL2** ensures portability

---

## Current Limitations & Technical Debt

### Performance
- **No spatial partitioning** for collision detection (O(n²) enemy-bullet checks)
- **No object pooling** for frequently created/destroyed entities
- **Immediate mode rendering** without batching optimization

### Architecture
- **Monolithic Game class** handles too many responsibilities
- **Global state** through Game instance passing
- **No component system** for entity composition
- **Hardcoded constants** scattered throughout code

### Features
- **Limited weapon variety** (only 3 types implemented)
- **No sound system** integration
- **Basic AI** for enemies (simple chase behavior)
- **No save/load** functionality
- **No settings/options** system

---

## Development Workflow

### Building the Project
```bash
# Windows (PowerShell)
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .

# macOS
./build-macos.sh
```

### Adding New Features
1. **Identify affected systems** (usually Game.cpp + entity classes)
2. **Update header files** with new member variables/functions
3. **Implement logic** in update/render loops
4. **Add input handling** if needed
5. **Test collision/interaction** with existing systems

### Code Patterns
```cpp
// Entity management pattern
entities.erase(std::remove_if(entities.begin(), entities.end(),
    [](const std::unique_ptr<Entity>& entity) {
        return !entity->isAlive();
    }), entities.end());

// Rendering pattern
void render(SDL_Renderer* renderer) {
    // Set draw color
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    
    // Draw shape or texture
    SDL_RenderFillRect(renderer, &rect);
    // OR
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}
```

---

## Conclusion

This is a **well-structured C++ game** that successfully implements core Brotato mechanics with room for significant expansion. The codebase demonstrates good SDL2 usage, proper memory management, and a solid foundation for adding new features. The modular design (separate classes for different entity types) makes it relatively easy to extend with new gameplay elements.

**Strengths:**
- Clean separation of concerns between entity types
- Comprehensive UI system with fallback rendering
- Proper resource management and cleanup
- Solid game loop implementation
- Good foundation for expansion

**Areas for Improvement:**
- Performance optimizations for scaling to more entities
- Code organization (break up large Game class)
- More sophisticated AI and game systems
- Audio integration
- Save/settings systems

The codebase provides an excellent foundation for implementing the many creative features outlined in the workshop documentation and beyond.
