# Original Game Project Analysis

## 🎮 Project Overview: **Monolithic Architecture**

Original Brotato implementation with single Game class containing all systems.

## 🏗️ Architecture Analysis

**Monolithic Design:**
```
src/
├── Game.h/cpp           - Main game loop, all systems
├── Player.h/cpp         - Player entity, stats, weapons
├── Enemy.h/cpp          - Base enemy class  
├── SlimeEnemy.h/cpp     - Specialized slime enemy
├── PebblinEnemy.h/cpp   - Specialized pebblin enemy  
├── Weapon.h/cpp         - Weapon system (3 types)
├── Shop.h/cpp           - Shop system with UI
├── Bullet.h/cpp         - Projectile system
├── ExperienceOrb.h/cpp  - XP collection system
├── Material.h/cpp       - Currency collection
└── Vector2.h/cpp        - Math utilities
```

**Characteristics:**
- Single Game class orchestrates everything
- Direct ownership via smart pointers
- No service locator or event system
- Tight coupling between systems
- Simple but not scalable for teams

## 💾 Core Systems Analysis

### 1. Game Class (Central Controller)
```cpp
class Game {
private:
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<SpawnIndicator> spawnIndicators;
    std::vector<std::unique_ptr<ExperienceOrb>> experienceOrbs;
    std::vector<std::unique_ptr<Material>> materials;
    std::unique_ptr<Shop> shop;
    
    // Wave system
    float waveTimer, waveDuration;
    bool waveActive;
    int wave, materialBag;
    
    // Core methods
    void spawnEnemies();
    void checkCollisions();
    void updateExperienceCollection();
    void updateMaterialCollection();
}
```

### 2. Wave System
- **Timer-based progression**: `waveTimer` tracks current wave
- **Duration scaling**: starts 20s → +5s per wave → max 60s
- **Shop integration**: wave end opens shop, pauses game
- **Material bag**: collects uncollected materials for next wave
- **Enemy scaling**: spawn rate increases with wave

### 3. Player System
```cpp
struct PlayerStats {
    int maxHealth = 100, damage = 10;
    float moveSpeed = 200, pickupRange = 50;
    float attackSpeed = 1.0f, range = 200;
    int armor = 0, materials = 50;
    float healthRegen = 0, dodgeChance = 0;
    int luck = 0;
}
```
- **Health system**: armor reduction, dodge chance  
- **Leveling**: `(Level + 3)²` XP formula
- **Weapon slots**: MAX_WEAPONS = 6
- **Auto-upgrades**: +1 HP per level, automatic weapon grants

### 4. Enemy System
**Inheritance hierarchy:**
- `Enemy` (base) - basic chasing behavior
- `SlimeEnemy` - specialized movement/stats
- `PebblinEnemy` - different stats/behavior

**Spawn system:**
- **Telegraph warnings**: 2-second red X indicators
- **Spawn rate scaling**: 1.0s → decreases by wave (min 0.2s)
- **Type distribution**: 
  - Wave 1: 50% slime, 50% base
  - Wave 2+: 40% slime, 40% pebblin, 20% base

### 5. Weapon System
**Only 3 weapon types:**
```cpp
enum class WeaponType {
    PISTOL,    // Balanced ranged
    SMG,       // High rate, low accuracy  
    MELEE_STICK // Area damage, knockback
}
```

**4 tiers per weapon:**
- Tier scaling affects damage/stats
- Player can hold up to 6 weapons
- Shop sells weapons directly to player

### 6. Shop System
**Core mechanics:**
- **4 items max** displayed simultaneously
- **Buy**: removes item, adds weapon to player
- **Lock**: item persists through reroll
- **Reroll**: regenerates non-locked items

**Pricing formulas:**
```cpp
// Item price
basePrice = 20 * tier + (wave * 5)

// Reroll price  
rerollIncrease = max(1, floor(0.40 * wave))
firstReroll = floor(wave * 0.75) + rerollIncrease
finalPrice = firstReroll + (rerollCount * rerollIncrease)
```

### 7. Collection Systems
**Experience Orbs:**
- **Magnetic pickup**: within `player.pickupRange`
- **XP formula**: `(level + 3)²` for next level
- **Auto leveling**: +1 max HP, some automatic weapons

**Materials:**
- **Drop chance formula**: 
  ```cpp
  baseChance = 100%
  reduction = (wave - 1) * 1.5%  
  finalChance = max(50%, baseChance - reduction)
  ```
- **Dual purpose**: materials give both XP and currency
- **Bag system**: uncollected materials stored for next wave

### 8. Combat System
**Collision detection:**
- **Player vs Enemy**: direct contact damage
- **Bullet vs Enemy**: projectile hits
- **Enemy bullets vs Player**: ranged enemy attacks

**Damage calculation:**
```cpp 
actualDamage = max(1, damage - armor)
if (dodgeRoll < dodgeChance) return; // Dodged
player.health -= actualDamage
```

## 🎯 Key Differences from Our Architecture

### Original (Monolithic)
✅ **Simple to understand**
✅ **Fast direct access** 
✅ **No overhead from services**
❌ **Hard to extend**
❌ **Cannot support team development**
❌ **Tight coupling**

### Our Architecture (Modular ECS)
✅ **Team-friendly modular design**
✅ **Scalable and maintainable**
✅ **Loose coupling via events**
❌ **More complex setup**
❌ **Service locator overhead**

## 📊 Feature Comparison

| Feature | Original | Our Implementation | Status |
|---------|----------|-------------------|--------|
| Wave System | Timer-based | Timer-based | ✅ Complete |
| Enemy Types | 3 types | 3 types | ✅ Complete |
| Weapons | 3 types, 4 tiers | 5 types, 4 tiers | ✅ Enhanced |
| Shop | Buy/Lock/Reroll | Buy/Lock/Reroll | ✅ Complete |
| Player Stats | 10 stats | 15+ stats | ✅ Enhanced |
| Collection | XP + Materials | XP + Materials | ✅ Complete |
| Telegraph | 2s red X | 2s red X | ✅ Complete |
| UI | TTF + Bitmap | TTF + Bitmap | ⚠️ Partial |

## 🔧 Technical Specifications

**Performance:**
- **Target resolution**: 1920x1080 fullscreen
- **Frame rate**: ~60 FPS
- **Memory**: Direct ownership, no pooling
- **Rendering**: Immediate mode SDL2

**Dependencies:**
- SDL2 (graphics, input, window)
- SDL2_image (PNG textures)  
- SDL2_ttf (font rendering)

**Build system:**
- Direct compilation, no CMake
- Manual dependency management

## 🎮 Gameplay Loop

1. **Wave start**: Timer begins, enemies spawn with telegraphs
2. **Combat phase**: Player fights, collects XP/materials  
3. **Wave end**: Uncollected materials → bag, shop opens
4. **Shop phase**: Buy weapons, reroll, close shop
5. **Next wave**: Timer resets, duration +5s, repeat

## 📈 Progression Systems

**Wave progression:** 20s → 25s → 30s → ... → 60s (max)
**Enemy scaling:** Spawn rate increases, more enemy types
**Weapon progression:** Shop offers higher tiers based on wave
**Player progression:** XP → levels → +1 HP, occasional weapons

---

**Summary:** Simple, effective monolithic design ideal for solo development but not scalable for teams. Our modular architecture provides same functionality with better maintainability and team development support.
