# Technical Architecture Details

## 🏗️ Detailed System Architecture

### Entity-Component-System Implementation

**Location:** `Gameplay/Entities/EntityManager.h`

**Core Components:**
```cpp
struct Transform {
    Vector2 position = Vector2(0, 0);
    float rotation = 0.0f;
    Vector2 scale = Vector2(1, 1);
};

struct Health {
    int current = 100;
    int maximum = 100; 
    float regeneration = 0.0f;
    bool invulnerable = false;
};

struct Player {
    Vector2 aimDirection = Vector2(1, 0);
    int level = 1;
    int experience = 0;
    int experienceToNextLevel = 150;
    int materials = 0;
    float pickupRange = 80.0f;
    float baseDamageMultiplier = 1.0f;
    float baseSpeedMultiplier = 1.0f;
};

struct Weapon {
    WeaponType type = WeaponType::PISTOL;
    WeaponTier tier = WeaponTier::TIER_1;
    int damage = 25;
    float fireRate = 2.0f;
    float range = 400.0f;
    float critChance = 0.05f;
    float critMultiplier = 2.0f;
    int pelletsPerShot = 1;
    float spread = 0.0f;
    bool isChargeWeapon = false;
    float maxChargeTime = 0.0f;
    float lastShotTime = 0.0f;
};
```

### Service Locator Pattern

**Location:** `Core/Engine/ServiceLocator.h`

```cpp
class ServiceLocator {
public:
    template<typename T>
    void registerService(std::shared_ptr<T> service);
    
    template<typename T>
    std::shared_ptr<T> getService();
    
private:
    std::unordered_map<std::type_index, std::shared_ptr<void>> services;
};
```

**Usage:**
- Renderer service registration
- Input manager access
- Event system integration

### Event System

**Location:** `Core/Events/EventManager.h`

**Key Events:**
```cpp
struct PlayerLevelUpEvent : public IEvent {
    int newLevel;
    int healthGain;
    int damageGain;
    float speedGain;
};

struct WaveCompletedEvent : public IEvent {
    int waveNumber;
    int enemiesKilled;
    float timeElapsed;
};

struct ShopOpenEvent : public IEvent {
    int playerMaterials;
    int waveNumber;
};
```

## 🔫 Weapon System Details

### Weapon Factory Configuration

**Location:** `Content/Weapons/WeaponFactory.cpp`

**Configuration Examples:**
```cpp
// Tier 1 Pistol
WeaponConfig tier1;
tier1.baseDamage = 20;
tier1.fireRate = 1.2f;
tier1.range = 400.0f;
tier1.critChance = 0.05f;
tier1.critMultiplier = 2.0f;
tier1.materialCost = 15;

// Tier 1 Shotgun  
WeaponConfig shotgun;
shotgun.baseDamage = 15; // Per pellet
shotgun.fireRate = 0.8f;
shotgun.pelletsPerShot = 5;
shotgun.spread = 0.3f;
shotgun.materialCost = 25;
```

### Melee Combat Implementation

**Location:** `Gameplay/Systems/ShootingSystem.cpp`

```cpp
void ShootingSystem::performMeleeAttack(const Vector2& playerPos, 
                                       const Vector2& aimDirection,
                                       const Weapon& weapon, 
                                       const Player& player) {
    // Calculate weapon tip position
    Vector2 weaponTip = playerPos + aimDirection * weapon.range;
    
    // Find enemies in damage radius
    float damageRadius = 40.0f;
    auto enemies = entityManager->getEntitiesWith<Transform, Health, Enemy>();
    
    for (EntityId enemyId : enemies) {
        auto enemyTransform = entityManager->getComponent<Transform>(enemyId);
        float distance = (enemyTransform->position - weaponTip).length();
        
        if (distance <= damageRadius) {
            // Apply damage and knockback
            applyDamageAndKnockback(enemyId, damage, playerPos);
        }
    }
}
```

## 🌊 Wave System Implementation

**Location:** `Gameplay/Systems/WaveSystem.cpp`

**Wave Timing Logic:**
```cpp
float WaveSystem::calculateWaveDuration(int waveNumber) {
    // Progressive timing: 20, 25, 30, 35, 40, 45, 50, 55, 60 (max)
    float baseDuration = 20.0f;
    float increment = 5.0f;
    float maxDuration = 60.0f;
    
    float duration = baseDuration + (waveNumber - 1) * increment;
    return std::min(duration, maxDuration);
}

void WaveSystem::startNextWave() {
    currentWave++;
    currentWaveDuration = calculateWaveDuration(currentWave);
    waveTimeRemaining = currentWaveDuration;
    
    // Trigger wave start event
    auto event = std::make_shared<WaveStartEvent>();
    event->waveNumber = currentWave;
    event->duration = currentWaveDuration;
    eventManager->publishEvent(event);
}
```

## 🏪 Shop System Mechanics

**Location:** `Gameplay/Systems/ShopSystem.cpp`

**Pricing Algorithm:**
```cpp
int ShopSystem::calculateWeaponPrice(WeaponType type, WeaponTier tier, int waveNumber) {
    auto config = weaponFactory->getWeaponConfig(type, tier);
    int basePrice = config.materialCost;
    
    // Wave-based price scaling
    int waveBonus = waveNumber * 2;
    
    return basePrice + waveBonus;
}

int ShopSystem::calculateRerollCost(int rerollCount, int waveNumber) {
    return 3 + (rerollCount * 2) + (waveNumber / 2);
}
```

**Shop Item Generation:**
```cpp
void ShopSystem::generateShopItems() {
    shopItems.clear();
    
    for (int i = 0; i < 4; i++) {
        ShopItem item;
        item.weaponType = getRandomWeaponType();
        item.tier = getRandomTier();
        item.price = calculateWeaponPrice(item.weaponType, item.tier, currentWave);
        item.isLocked = false;
        
        shopItems.push_back(item);
    }
}
```

## 📊 Performance Optimizations

### Memory Management
- **Component Pools:** Efficient ECS storage with sparse sets
- **Smart Pointers:** RAII with `std::shared_ptr`/`std::unique_ptr`
- **Texture Caching:** Prevent duplicate asset loading
- **Object Pooling:** Reuse bullet/enemy entities

### Collision Detection
**Current:** O(n²) brute force for simplicity
**Planned:** Spatial partitioning (QuadTree) for O(n log n)

```cpp
// Current collision check
for (auto bullet : bullets) {
    for (auto enemy : enemies) {
        if (isColliding(bullet, enemy)) {
            handleCollision(bullet, enemy);
        }
    }
}
```

### Rendering Pipeline
- **Batched Rendering:** Group similar draw calls
- **Culling:** Only render visible entities  
- **Texture Atlas:** Reduce texture switches (planned)

## 🔧 Build System Details

### CMake Configuration
```cmake
# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(BrotatoGame)

# Find packages via vcpkg
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)  
find_package(SDL2_ttf REQUIRED)

# Module libraries
add_subdirectory(Core)
add_subdirectory(Rendering)
add_subdirectory(Input)
add_subdirectory(Gameplay)
add_subdirectory(Content)

# Main executable
add_executable(BrotatoGame main.cpp)
target_link_libraries(BrotatoGame 
    BrotatoCore 
    BrotatoRendering 
    BrotatoInput
    BrotatoGameplay
    BrotatoContent
)
```

### Dependency Management
**vcpkg.json:**
```json
{
  "dependencies": [
    "sdl2",
    "sdl2-image", 
    "sdl2-ttf"
  ]
}
```

## 🎯 Testing Framework (Future)

**Planned Test Structure:**
```cpp
// Unit tests for ECS
TEST(EntityManager, CreateEntity) {
    EntityManager em;
    EntityId entity = em.createEntity();
    EXPECT_NE(entity, INVALID_ENTITY_ID);
}

// Integration tests for gameplay
TEST(WeaponSystem, PistolDamage) {
    WeaponFactory factory;
    auto pistol = factory.createWeapon(WeaponType::PISTOL, WeaponTier::TIER_1);
    EXPECT_EQ(pistol.damage, 20);
}
```

## 🚀 Deployment

### Asset Pipeline
```bash
# Assets automatically copied to build directory
assets/
├── weapons/     -> build/Debug/assets/weapons/
├── enemies/     -> build/Debug/assets/enemies/
└── fonts/       -> build/Debug/assets/fonts/
```

### Distribution
- **Windows:** Single executable with DLL dependencies
- **Cross-platform:** CMake toolchain for macOS/Linux
- **Future:** Steam distribution pipeline
