# Development Plan - Phase 03
## Extended Features for Brotato MVP Game

**Document Version**: 1.0  
**Created**: December 2024  
**Development Team**: 6 participants

---

## 👥 Team Tasks Overview

| Participant | Tasks | Priority | Complexity | Duration | Dependencies |
|------------|-------|----------|------------|----------|--------------|
| **Sasha Shukala** | Screenshot Mode, Main Menu | Medium/High | Easy/Hard | 5-6 days | None |
| **Viktor** | Shotgun, Weapon Combining | High | Medium/Hard | 6-8 days | None |
| **Sergey K** | Railgun | Medium | Medium | 2-3 days | None |
| **Sergey Shurygin** | Healing & Vampirism, Mobile Port | Medium/Low | Medium/Very Hard | 10-14 days | Main Menu |
| **Artem** | Stats on Levelup | Medium | Easy | 1-2 days | None |
| **TBD** | Mac Build | High | Medium | 2-3 days | None |

---

# 🎯 Individual Task Assignments

## 👤 Sasha Shukala - UI & Menu Systems

### Task 1: Screenshot Mode (Priority: Low, Duration: 1 day)

#### Description
Toggle mode that hides all UI elements for clean gameplay screenshots.

#### Technical Requirements
- **Hotkey**: F12 or Print Screen
- **Functionality**: Complete UI hiding while gameplay continues
- **Indicator**: Small camera icon in corner when active

#### Files to Modify
```
src/Game.h (add screenshotMode flag)
src/Game.cpp (input handling and conditional rendering)
```

#### Implementation Steps

**Step 1: Add State Flag (0.2 days)**
```cpp
// In Game.h
class Game {
private:
    bool screenshotMode = false;
    bool screenshotKeyPressed = false;
    
public:
    void toggleScreenshotMode();
    bool isScreenshotMode() const { return screenshotMode; }
};
```

**Step 2: Input Handling (0.3 days)**
```cpp
// In Game.cpp - handleEvents()
void Game::handleEvents() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    // Toggle screenshot mode with F12
    if (keyState[SDL_SCANCODE_F12] && !screenshotKeyPressed) {
        toggleScreenshotMode();
        screenshotKeyPressed = true;
    } else if (!keyState[SDL_SCANCODE_F12]) {
        screenshotKeyPressed = false;
    }
}
```

**Step 3: Conditional UI Rendering (0.5 days)**
```cpp
// In Game.cpp - render()
void Game::render() {
    // ... render game objects ...
    
    if (!screenshotMode) {
        renderUI();
    } else {
        // Show small camera icon
        renderScreenshotIndicator();
    }
}
```

#### Required Resources
- **Graphics**: Small camera icon (16x16 px)
- **File**: `assets/ui/camera_icon.png`

#### Acceptance Criteria
- [ ] F12 toggles screenshot mode
- [ ] All UI hidden in screenshot mode
- [ ] Gameplay continues normally
- [ ] Visual indicator shows active mode
- [ ] F12 again restores UI

---

### Task 2: Main Menu with Settings (Priority: High, Duration: 4-5 days)

#### Description
Complete main menu system with game settings and configuration saving.

#### Technical Requirements
- **Menu Options**: Start Game, Settings, Exit
- **Settings**: Volume, Resolution, Controls
- **Persistence**: Settings saved between sessions

#### Files to Create/Modify
```
src/GameState.h (new - game state system)
src/MainMenu.h (new)
src/MainMenu.cpp (new)
src/Settings.h (new)
src/Settings.cpp (new)
src/Game.h (integrate states)
src/Game.cpp (update game loop)
settings.ini (new - configuration file)
```

#### Implementation Steps

**Step 1: Game State System (1 day)**
```cpp
// GameState.h
enum class GameState {
    MAIN_MENU,
    IN_GAME,
    PAUSED,
    SETTINGS,
    GAME_OVER
};

class GameStateManager {
private:
    GameState currentState = GameState::MAIN_MENU;
    GameState previousState = GameState::MAIN_MENU;
    
public:
    void changeState(GameState newState);
    GameState getCurrentState() const { return currentState; }
    void returnToPrevious();
};
```

**Step 2: Main Menu Implementation (1.5 days)**
```cpp
// MainMenu.h
class MainMenu {
private:
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* logoTexture = nullptr;
    
    enum class MenuOption {
        START_GAME,
        SETTINGS,
        EXIT
    };
    
    MenuOption selectedOption = MenuOption::START_GAME;
    bool keyPressed[3] = {false}; // Input handling
    
public:
    void loadAssets(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer);
    void handleInput(const Uint8* keyState);
    MenuOption getSelectedOption() const;
    bool isOptionSelected() const;
};
```

**Step 3: Settings System (2 days)**
```cpp
// Settings.h
struct GameSettings {
    float masterVolume = 1.0f;
    float sfxVolume = 1.0f;
    int windowWidth = 1920;
    int windowHeight = 1080;
    bool fullscreen = true;
    
    // Controls
    SDL_Scancode moveUp = SDL_SCANCODE_W;
    SDL_Scancode moveDown = SDL_SCANCODE_S;
    SDL_Scancode moveLeft = SDL_SCANCODE_A;
    SDL_Scancode moveRight = SDL_SCANCODE_D;
    SDL_Scancode shoot = SDL_SCANCODE_SPACE;
    
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void setDefaults();
};
```

**Step 4: Integration with Main Game (1 day)**
```cpp
// In Game.cpp - modified run() method
void Game::run() {
    while (running) {
        handleEvents();
        
        switch (stateManager.getCurrentState()) {
            case GameState::MAIN_MENU:
                mainMenu.handleInput(keyState);
                if (mainMenu.isOptionSelected()) {
                    handleMenuSelection(mainMenu.getSelectedOption());
                }
                break;
                
            case GameState::IN_GAME:
                update(deltaTime);
                break;
                
            case GameState::SETTINGS:
                settingsMenu.handleInput(keyState);
                if (settingsMenu.isDone()) {
                    stateManager.returnToPrevious();
                }
                break;
        }
        
        render();
    }
}
```

#### Required Resources
- **Graphics**:
  - `assets/ui/menu_background.png` (1920x1080)
  - `assets/ui/game_logo.png` (400x150)
  - `assets/ui/button_normal.png` (200x60)
  - `assets/ui/button_selected.png` (200x60)
  - `assets/ui/slider_bg.png` (200x20)
  - `assets/ui/slider_handle.png` (20x20)

#### Acceptance Criteria
- [ ] Game starts with main menu
- [ ] "Start Game", "Settings", "Exit" buttons work
- [ ] Settings menu allows volume/resolution changes
- [ ] Settings persist between game sessions
- [ ] ESC returns to previous menu
- [ ] Keyboard and mouse navigation

---

## 👤 Viktor - Weapon Systems

### Task 1: Shotgun Implementation (Priority: Medium, Duration: 2-3 days)

#### Description
Shotgun weapon with spread pattern, effective at close range against groups.

#### Technical Requirements
- **Fire Pattern**: 5-7 bullets in cone spread
- **Damage**: Medium per bullet, high total
- **Range**: Limited distance
- **Spread**: 15-30 degrees

#### Files to Modify
```
src/Weapon.h (add WeaponType::SHOTGUN)
src/Weapon.cpp (spread fire logic)
src/Bullet.cpp (short-lived bullets)
```

#### Implementation Steps

**Step 1: Add Weapon Type (0.5 days)**
```cpp
// In Weapon.h
enum class WeaponType {
    PISTOL,
    SMG,
    MELEE_STICK,
    RAILGUN,
    SHOTGUN  // New type
};
```

**Step 2: Spread Fire Logic (1.5 days)**
```cpp
// In Weapon.cpp
void Weapon::fireShotgun(const Vector2& weaponPos, const Vector2& direction, 
                        std::vector<std::unique_ptr<Bullet>>& bullets,
                        const Player& player) {
    const int pelletCount = 6;
    const float spreadAngle = 25.0f; // degrees
    
    for (int i = 0; i < pelletCount; i++) {
        float angleOffset = (i - pelletCount/2.0f) * (spreadAngle / pelletCount);
        Vector2 pelletDirection = rotateVector(direction, angleOffset);
        
        auto bullet = std::make_unique<Bullet>(weaponPos, pelletDirection, 
                                             calculateDamage(player) / 2);
        bullet->setLifetime(0.3f); // Short range
        bullets.push_back(std::move(bullet));
    }
}
```

**Step 3: Stats and Balancing (1 day)**
```cpp
void Weapon::initializeShotgunStats() {
    switch (tier) {
        case WeaponTier::TIER_1:
            stats.baseDamage = 8;       // Per pellet
            stats.attackSpeed = 1.8f;   // Slower than pistol
            stats.range = 150;          // Short range
            break;
        case WeaponTier::TIER_2:
            stats.baseDamage = 12;
            stats.attackSpeed = 1.6f;
            stats.range = 200;
            break;
        // ... other tiers
    }
}
```

#### Required Resources
- **Graphics**:
  - `assets/weapons/shotgun.png` (48x48)
  - `assets/weapons/shotgun2.png` (48x48) - Tier 2
  - `assets/weapons/shotgun3.png` (48x48) - Tier 3+

#### Acceptance Criteria
- [ ] Shotgun fires multiple bullets with spread
- [ ] Effective at short range
- [ ] Damage decreases with distance
- [ ] Available in shop at different tiers
- [ ] Muzzle flash visual effect

---

### Task 2: Weapon Upgrading/Combining System (Priority: High, Duration: 4-5 days)

#### Description
System for combining 2-3 identical weapons into higher tier versions.

#### Technical Requirements
- **Mechanics**: Combine 3 same weapons → next tier
- **Interface**: Shop integration or inventory drag & drop
- **Effects**: Visual indicators for combinable weapons

#### Files to Modify
```
src/Player.h (inventory system expansion)
src/Player.cpp (combining logic)
src/Shop.h (UI for combining)
src/Shop.cpp (combine interface)
src/Weapon.h (tier system enhancement)
```

#### Implementation Steps

**Step 1: Inventory System Enhancement (1.5 days)**
```cpp
// In Player.h
class Player {
private:
    struct WeaponSlot {
        std::unique_ptr<Weapon> weapon;
        bool canCombine = false;
        int combineCount = 0; // How many of this type/tier we have
    };
    
    std::vector<WeaponSlot> weaponSlots;
    
public:
    bool canCombineWeapons(WeaponType type, WeaponTier tier) const;
    std::unique_ptr<Weapon> combineWeapons(WeaponType type, WeaponTier tier);
    void updateCombinePossibilities();
    int getWeaponCount(WeaponType type, WeaponTier tier) const;
};
```

**Step 2: Combining Logic (2 days)**
```cpp
// In Player.cpp
bool Player::canCombineWeapons(WeaponType type, WeaponTier tier) const {
    int count = getWeaponCount(type, tier);
    return count >= 3 && tier != WeaponTier::TIER_4; // Max tier check
}

std::unique_ptr<Weapon> Player::combineWeapons(WeaponType type, WeaponTier tier) {
    // Remove 3 weapons of same type/tier
    int removed = 0;
    for (auto& slot : weaponSlots) {
        if (removed >= 3) break;
        if (slot.weapon && 
            slot.weapon->getType() == type && 
            slot.weapon->getTier() == tier) {
            slot.weapon.reset();
            removed++;
        }
    }
    
    // Create upgraded version
    WeaponTier newTier = static_cast<WeaponTier>(static_cast<int>(tier) + 1);
    auto upgradedWeapon = std::make_unique<Weapon>(type, newTier);
    upgradedWeapon->initialize(renderer);
    
    return upgradedWeapon;
}
```

**Step 3: Shop UI Integration (1.5 days)**
```cpp
// In Shop.cpp
void Shop::renderWeaponCombining(SDL_Renderer* renderer, int x, int y) {
    renderText(renderer, "COMBINE WEAPONS", x, y, 2);
    y += 40;
    
    // Show available combinations
    for (auto type : {WeaponType::PISTOL, WeaponType::SMG, WeaponType::SHOTGUN}) {
        for (auto tier : {WeaponTier::TIER_1, WeaponTier::TIER_2, WeaponTier::TIER_3}) {
            if (gameRef->getPlayer()->canCombineWeapons(type, tier)) {
                renderCombineButton(renderer, type, tier, x, y);
                y += 60;
            }
        }
    }
}

void Shop::handleCombineInput(const Uint8* keyState, Player& player) {
    // Handle combine button presses
    if (keyState[SDL_SCANCODE_C] && selectedCombineOption >= 0) {
        auto upgradedWeapon = player.combineWeapons(selectedType, selectedTier);
        if (upgradedWeapon) {
            player.addWeapon(std::move(upgradedWeapon));
            showCombineEffect();
        }
    }
}
```

#### Required Resources
- **Graphics**:
  - `assets/ui/combine_button.png` (80x30)
  - `assets/ui/combine_arrow.png` (32x16)
  - `assets/ui/combine_glow.png` (64x64) - Effect overlay
  - **Sound Effects** (if audio system exists):
    - `assets/sounds/weapon_combine.wav`

#### Acceptance Criteria
- [ ] 3 identical weapons can be combined
- [ ] Combining creates next tier weapon
- [ ] UI shows available combinations
- [ ] Combine button/hotkey works
- [ ] Visual effects on successful combine
- [ ] Cannot combine Tier 4 weapons

---

## 👤 Sergey K - Advanced Weapons

### Task: Railgun Implementation (Priority: Medium, Duration: 2-3 days)

#### Description
High-tech weapon with instant piercing shot through all enemies in line.

#### Technical Requirements
- **Damage Type**: High single-shot damage
- **Special**: Pierces through all enemies on line
- **Effects**: Laser beam, electric sparks
- **Reload**: Long cooldown (2-3 seconds)

#### Files to Modify
```
src/Weapon.h (add WeaponType::RAILGUN)
src/Weapon.cpp (railgun logic implementation)
src/Bullet.h (add BulletType::RAILGUN_BEAM)
src/Bullet.cpp (piercing damage)
src/Game.cpp (special railgun bullet handling)
```

#### Implementation Steps

**Step 1: Weapon System Extension (1 day)**
```cpp
// In Weapon.h
enum class WeaponType {
    PISTOL,
    SMG,
    MELEE_STICK,
    RAILGUN  // New type
};

// In Weapon.cpp
void Weapon::initializeRailgunStats() {
    stats.baseDamage = 80;      // High damage
    stats.attackSpeed = 2.5f;   // Slow reload
    stats.range = 2000;         // Across entire screen
    stats.critChance = 0.25f;   // High crit chance
    stats.critMultiplier = 3.0f;
}
```

**Step 2: Special Bullet System (1 day)**
```cpp
// In Bullet.h
enum class BulletType {
    NORMAL,
    RAILGUN_BEAM
};

class Bullet {
private:
    BulletType bulletType;
    int remainingPierces;
    bool isRailgunBeam;
    
public:
    bool canPierce() const;
    void onHitEnemy();
    bool isInstantHit() const { return bulletType == BulletType::RAILGUN_BEAM; }
};

// In Bullet.cpp
void Bullet::onHitEnemy() {
    if (bulletType == BulletType::RAILGUN_BEAM) {
        // Railgun pierces through all enemies, never stops
        return;
    }
    // Normal bullet logic
}
```

**Step 3: Visual Effects (0.5-1 day)**
```cpp
// In Game.cpp
void Game::fireRailgun(const Vector2& start, const Vector2& direction) {
    // Calculate end point (edge of screen)
    Vector2 end = start + direction * 2000;
    
    // Hit all enemies on line instantly
    for (auto& enemy : enemies) {
        if (isPointOnLine(start, end, enemy->getPosition(), 10.0f)) {
            enemy->takeDamage(railgunDamage);
        }
    }
    
    // Store beam for rendering
    railgunBeams.push_back({start, end, 0.2f}); // 0.2s duration
}

void Game::renderRailgunBeam(const Vector2& start, const Vector2& end) {
    // Main beam
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Cyan
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
    
    // Glow effect
    for (int i = 1; i <= 3; i++) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 128/i);
        // Render additional lines for glow effect
        SDL_RenderDrawLine(renderer, start.x + i, start.y, end.x + i, end.y);
        SDL_RenderDrawLine(renderer, start.x - i, start.y, end.x - i, end.y);
    }
}
```

#### Required Resources
- **Graphics**:
  - `assets/weapons/railgun.png` (48x48)
  - `assets/weapons/railgun2.png` (48x48) - Tier 2+
  - `assets/effects/railgun_charge.png` (32x32) - Charging effect
- **Sound Effects** (if audio system exists):
  - `assets/sounds/railgun_charge.wav`
  - `assets/sounds/railgun_fire.wav`

#### Acceptance Criteria
- [ ] Railgun available in shop (Tier 3-4)
- [ ] Instant shot through all enemies on line
- [ ] High damage with long reload
- [ ] Visual laser beam effect
- [ ] Charging visual effect before shot

---

## 👤 Sergey Shurygin - Health Systems & Mobile

### Task 1: Healing Items & Vampirism (Priority: Medium, Duration: 3-4 days)

#### Description
Health restoration system including item drops and vampirism stat.

#### Technical Requirements
- **Health Items**: Medkits dropping from enemies
- **Vampirism**: Player stat restoring health on kills
- **Visualization**: Healing effects, vampirism indicators

#### Files to Create/Modify
```
src/Player.h (add vampirism stat)
src/Player.cpp (healing logic)
src/Game.cpp (medkit spawning, vampirism handling)
src/HealthPickup.h (new class)
src/HealthPickup.cpp (medkit logic)
```

#### Implementation Steps

**Step 1: Stats System Extension (0.5 days)**
```cpp
// In Player.h
struct PlayerStats {
    // ... existing stats ...
    float vampirism = 0.0f;        // 0.0 - 1.0 (% of damage as healing)
    float healingBonus = 1.0f;     // Healing effectiveness multiplier
};
```

**Step 2: Health Pickup Class (1 day)**
```cpp
// HealthPickup.h
class HealthPickup {
private:
    Vector2 position;
    int healAmount;
    float lifetime;
    float bobTimer;
    SDL_Texture* texture;
    
public:
    HealthPickup(const Vector2& pos, int amount);
    ~HealthPickup();
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void loadTexture(SDL_Renderer* renderer);
    
    Vector2 getPosition() const { return position; }
    int getHealAmount() const { return healAmount; }
    bool shouldDestroy() const { return lifetime <= 0; }
};

// HealthPickup.cpp implementation
HealthPickup::HealthPickup(const Vector2& pos, int amount) 
    : position(pos), healAmount(amount), lifetime(30.0f), 
      bobTimer(0.0f), texture(nullptr) {
}

void HealthPickup::update(float deltaTime) {
    lifetime -= deltaTime;
    bobTimer += deltaTime * 3.0f; // Bob animation speed
    
    // Floating animation
    position.y += sin(bobTimer) * 0.5f;
}
```

**Step 3: Vampirism Logic (1 day)**
```cpp
// In Player.cpp
void Player::onEnemyKilled(int damageDealt) {
    if (stats.vampirism > 0) {
        int healAmount = static_cast<int>(damageDealt * stats.vampirism);
        heal(healAmount);
        
        // Visual vampirism effect
        showVampirismEffect();
    }
}

void Player::heal(int amount) {
    int actualHeal = static_cast<int>(amount * stats.healingBonus);
    int oldHealth = health;
    health = std::min(health + actualHeal, stats.maxHealth);
    
    // Show healing numbers
    if (health > oldHealth) {
        showHealingEffect(health - oldHealth);
    }
}
```

**Step 4: Game Integration (1 day)**
```cpp
// In Game.cpp
void Game::spawnHealthPickups(const Vector2& enemyPosition) {
    // 5% chance to drop medkit
    if (rand() % 100 < 5) {
        int healAmount = 15 + (wave * 2); // More healing in later waves
        healthPickups.push_back(
            std::make_unique<HealthPickup>(enemyPosition, healAmount)
        );
    }
}

void Game::updateHealthPickups(float deltaTime) {
    for (auto it = healthPickups.begin(); it != healthPickups.end();) {
        (*it)->update(deltaTime);
        
        // Check player pickup
        float distance = ((*it)->getPosition() - player->getPosition()).length();
        if (distance < player->getStats().pickupRange) {
            player->heal((*it)->getHealAmount());
            it = healthPickups.erase(it);
        } else if ((*it)->shouldDestroy()) {
            it = healthPickups.erase(it);
        } else {
            ++it;
        }
    }
}
```

**Step 5: Shop Integration (0.5 days)**
```cpp
// Add vampirism items to shop
struct VampirismItem : public ShopItem {
    VampirismItem() {
        name = "Vampirism";
        description = "+10% lifesteal on kill";
        price = 25;
        // Apply to player stats on purchase
    }
};
```

#### Required Resources
- **Graphics**:
  - `assets/ui/healthpack.png` (24x24)
  - `assets/effects/heal_effect.png` (32x32)
  - `assets/effects/vampirism_effect.png` (48x48)
  - `assets/ui/vampirism_icon.png` (16x16) - UI indicator

#### Acceptance Criteria
- [ ] Medkits drop from killed enemies
- [ ] Medkits restore health when picked up
- [ ] Vampirism works on enemy kills
- [ ] Visual healing effects
- [ ] Vampirism items available in shop
- [ ] Healing doesn't exceed max health

---

### Task 2: Mobile Platforms Port (Priority: Low, Duration: 7-10 days)

#### Description
Port game to mobile platforms with touch controls (Android priority).

#### Technical Requirements
- **Platforms**: Android (priority), iOS (optional)
- **Controls**: Virtual joystick, touch shooting
- **UI Adaptation**: Scaling for different screen sizes

#### Files to Create/Modify
```
CMakeLists.txt (Android toolchain)
src/InputManager.h (new - input abstraction)
src/InputManager.cpp (touch event handling)
src/TouchControls.h (new)
src/TouchControls.cpp (virtual controls)
src/Game.cpp (mobile adaptations)
android/ (new folder - Android project)
```

#### Implementation Steps

**Step 1: Android SDK/NDK Setup (1 day)**
```bash
# Install Android SDK, NDK, CMake
# Create Android project with native activity
# Setup gradle build system
```

**Step 2: Input System Abstraction (2 days)**
```cpp
// InputManager.h
class InputManager {
public:
    virtual ~InputManager() = default;
    virtual Vector2 getMovementInput() = 0;
    virtual Vector2 getAimDirection() = 0;
    virtual bool isShootPressed() = 0;
    virtual bool isMenuPressed() = 0;
    virtual void update(float deltaTime) = 0;
};

class DesktopInputManager : public InputManager {
    // PC implementation
};

class MobileInputManager : public InputManager {
    // Mobile implementation with touch
};
```

**Step 3: Touch Controls (2-3 days)**
```cpp
// TouchControls.h
class TouchControls {
private:
    struct TouchButton {
        SDL_Rect bounds;
        bool pressed;
        SDL_Texture* texture;
        Vector2 center;
    };
    
    TouchButton moveJoystick;
    TouchButton shootButton;
    Vector2 joystickCurrent;
    bool joystickActive;
    
public:
    void handleTouchEvent(SDL_TouchFingerEvent& event);
    void render(SDL_Renderer* renderer);
    void loadTextures(SDL_Renderer* renderer);
    
    Vector2 getMovementVector() const;
    bool isShootPressed() const;
};
```

**Step 4: Mobile UI Adaptation (2 days)**
```cpp
// In Game.cpp
void Game::adaptForMobile() {
    // Scale UI elements for touch
    uiScale = 1.5f;
    
    // Reposition elements for thumb accessibility
    healthBarPosition = Vector2(50, 100);
    
    // Initialize touch controls
    touchControls = std::make_unique<TouchControls>();
    touchControls->loadTextures(renderer);
}
```

**Step 5: Build System and Testing (2-3 days)**
```cmake
# Android CMakeLists.txt
cmake_minimum_required(VERSION 3.18)
project(BrotatoAndroid)

find_library(log-lib log)
find_library(android-lib android)

find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)

target_link_libraries(BrotatoAndroid
    ${log-lib}
    ${android-lib}
    SDL2::SDL2
    SDL2_image::SDL2_image
)
```

#### Required Resources
- **Graphics**:
  - `assets/mobile/joystick_bg.png` (128x128)
  - `assets/mobile/joystick_knob.png` (64x64)
  - `assets/mobile/shoot_button.png` (96x96)
  - `assets/mobile/menu_button.png` (48x48)
- **Android Project Structure**:
  - `android/app/src/main/AndroidManifest.xml`
  - `android/app/build.gradle`
  - `android/gradle.properties`

#### Acceptance Criteria
- [ ] APK builds without errors
- [ ] Game runs on Android device
- [ ] Virtual joystick controls movement
- [ ] Touch shooting works correctly
- [ ] UI scaled appropriately for mobile
- [ ] Performance acceptable (30+ FPS)

---

## 👤 Artem - Player Progression

### Task: Improve Stats on Levelup (Priority: Medium, Duration: 1-2 days)

#### Description
Enhanced leveling system with choice of 3 random upgrades on level up.

#### Technical Requirements
- **Upgrade Choice**: 3 random improvements to choose from
- **Variety**: Different types of bonuses (damage, speed, health, etc.)
- **Balance**: Balanced improvements for different playstyles

#### Files to Create/Modify
```
src/LevelUpMenu.h (new)
src/LevelUpMenu.cpp (upgrade selection menu)
src/Player.h (expanded stats system)
src/Player.cpp (bonus application)
src/Game.cpp (pause game on level up)
```

#### Implementation Steps

**Step 1: Upgrade System Design (1 day)**
```cpp
// LevelUpMenu.h
enum class StatUpgradeType {
    DAMAGE,
    MAX_HEALTH,
    MOVE_SPEED,
    ATTACK_SPEED,
    PICKUP_RANGE,
    ARMOR,
    HEALTH_REGEN,
    VAMPIRISM,
    CRIT_CHANCE,
    LUCK
};

struct StatUpgrade {
    StatUpgradeType type;
    std::string name;
    std::string description;
    float value;
    
    void applyToPlayer(Player& player);
};

class LevelUpMenu {
private:
    std::vector<StatUpgrade> availableUpgrades;
    int selectedOption = 0;
    bool isActive = false;
    SDL_Texture* backgroundTexture = nullptr;
    
public:
    void generateRandomUpgrades(int playerLevel);
    void render(SDL_Renderer* renderer);
    void handleInput(const Uint8* keyState);
    
    StatUpgrade getSelectedUpgrade();
    bool isMenuActive() const { return isActive; }
    void activate() { isActive = true; }
    void deactivate() { isActive = false; }
};
```

**Step 2: Random Upgrade Generation (0.5 days)**
```cpp
// In LevelUpMenu.cpp
void LevelUpMenu::generateRandomUpgrades(int playerLevel) {
    availableUpgrades.clear();
    
    // Define all possible upgrades
    std::vector<StatUpgrade> upgradePool = {
        {StatUpgradeType::DAMAGE, "Damage +5", "Increase weapon damage", 5.0f},
        {StatUpgradeType::MAX_HEALTH, "Max Health +20", "Increase maximum health", 20.0f},
        {StatUpgradeType::MOVE_SPEED, "Speed +15", "Move faster", 15.0f},
        {StatUpgradeType::ATTACK_SPEED, "Attack Speed +10%", "Shoot faster", 0.1f},
        {StatUpgradeType::PICKUP_RANGE, "Pickup Range +10", "Collect items from farther", 10.0f},
        {StatUpgradeType::ARMOR, "Armor +2", "Reduce incoming damage", 2.0f},
        {StatUpgradeType::HEALTH_REGEN, "Health Regen +1", "Regenerate health over time", 1.0f},
        {StatUpgradeType::VAMPIRISM, "Vampirism +5%", "Heal on enemy kill", 0.05f},
        {StatUpgradeType::CRIT_CHANCE, "Crit Chance +5%", "Higher critical hit chance", 0.05f},
        {StatUpgradeType::LUCK, "Luck +1", "Better drops and effects", 1.0f}
    };
    
    // Randomly select 3 upgrades
    std::shuffle(upgradePool.begin(), upgradePool.end(), randomGenerator);
    for (int i = 0; i < 3 && i < upgradePool.size(); i++) {
        availableUpgrades.push_back(upgradePool[i]);
    }
}
```

**Step 3: Upgrade Application (0.5 days)**
```cpp
// In LevelUpMenu.cpp
void StatUpgrade::applyToPlayer(Player& player) {
    PlayerStats& stats = player.getStats();
    
    switch (type) {
        case StatUpgradeType::DAMAGE:
            stats.damage += static_cast<int>(value);
            break;
        case StatUpgradeType::MAX_HEALTH:
            stats.maxHealth += static_cast<int>(value);
            player.heal(static_cast<int>(value)); // Also heal
            break;
        case StatUpgradeType::MOVE_SPEED:
            stats.moveSpeed += value;
            break;
        case StatUpgradeType::ATTACK_SPEED:
            stats.attackSpeed *= (1.0f + value); // Multiplicative bonus
            break;
        case StatUpgradeType::PICKUP_RANGE:
            stats.pickupRange += value;
            break;
        case StatUpgradeType::ARMOR:
            stats.armor += static_cast<int>(value);
            break;
        case StatUpgradeType::HEALTH_REGEN:
            stats.healthRegen += value;
            break;
        case StatUpgradeType::VAMPIRISM:
            stats.vampirism += value;
            break;
        case StatUpgradeType::CRIT_CHANCE:
            stats.critChance += value;
            break;
        case StatUpgradeType::LUCK:
            stats.luck += static_cast<int>(value);
            break;
    }
}
```

#### Required Resources
- **Graphics**:
  - `assets/ui/levelup_bg.png` (600x400)
  - `assets/ui/upgrade_card.png` (180x120)
  - `assets/ui/upgrade_card_selected.png` (180x120)
  - `assets/icons/` folder with 16x16 icons for each stat type:
    - `damage_icon.png`
    - `health_icon.png`
    - `speed_icon.png`
    - `armor_icon.png`
    - etc.

#### Acceptance Criteria
- [ ] Game pauses on level up and shows menu
- [ ] 3 random upgrades offered
- [ ] Selected upgrade applies to player
- [ ] Upgrades are balanced and varied
- [ ] Menu UI is clear and intuitive

---

## 👤 TBD - Platform Support

### Task: Mac Build Support (Priority: High, Duration: 2-3 days)

#### Description
Adapt project for building and running on macOS with Xcode and Homebrew.

#### Technical Requirements
- **Build System**: CMake + vcpkg/Homebrew
- **Compiler**: Clang (Apple)
- **Dependencies**: SDL2, SDL2_image, SDL2_ttf via Homebrew

#### Files to Create/Modify
```
CMakeLists.txt (macOS adaptations)
vcpkg.json (add macOS triplet)
build-mac.sh (new build script)
install-deps-mac.sh (new dependency script)
README.md (update build instructions)
```

#### Implementation Steps

**Step 1: Build Environment Setup (0.5 days)**
```bash
# install-deps-mac.sh
#!/bin/bash
echo "Installing dependencies for macOS..."

# Install Homebrew if not present
if ! command -v brew &> /dev/null; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Install SDL2 libraries
brew install sdl2 sdl2_image sdl2_ttf cmake

echo "Dependencies installed successfully!"
```

**Step 2: CMake Adaptation (1 day)**
```cmake
# Add to CMakeLists.txt
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
    
    # Find SDL2 using pkg-config on macOS
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    
    # Link libraries
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )
    
    # Include directories
    target_include_directories(${PROJECT_NAME} 
        PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )
else()
    # Windows vcpkg configuration
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    )
endif()
```

**Step 3: Code Compatibility (1 day)**
```cpp
// Add platform-specific resource handling
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

std::string getResourcePath(const std::string& resource) {
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFStringRef resourceName = CFStringCreateWithCString(NULL, resource.c_str(), kCFStringEncodingUTF8);
    CFURLRef resourceURL = CFBundleCopyResourceURL(mainBundle, resourceName, NULL, NULL);
    
    if (resourceURL) {
        CFStringRef path = CFURLCopyFileSystemPath(resourceURL, kCFURLPOSIXPathStyle);
        char pathBuffer[PATH_MAX];
        CFStringGetCString(path, pathBuffer, PATH_MAX, kCFStringEncodingUTF8);
        
        CFRelease(path);
        CFRelease(resourceURL);
        CFRelease(resourceName);
        
        return std::string(pathBuffer);
    }
    
    CFRelease(resourceName);
    return resource; // Fallback to relative path
}
#endif
```

**Step 4: Build Script and Testing (0.5-1 day)**
```bash
# build-mac.sh
#!/bin/bash
echo "Building Brotato Game for macOS..."

# Check dependencies
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found. Please install dependencies first."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Game executable: ./BrotatoGame"
else
    echo "Build failed!"
    exit 1
fi
```

#### Required Resources
- **Documentation**:
  - Update README.md with macOS build instructions
  - Create macOS troubleshooting section
- **CI/CD**:
  - GitHub Actions workflow for macOS builds
  - Automated testing on macOS

#### Acceptance Criteria
- [ ] Project builds on macOS without errors
- [ ] Game runs correctly on macOS
- [ ] All resources load properly
- [ ] Created .app bundle for distribution
- [ ] Documentation updated

---

## 📚 Shared Resources & Assets

### Graphics Assets Required

#### UI Elements
```
assets/ui/
├── camera_icon.png (16x16) - Screenshot mode indicator
├── menu_background.png (1920x1080) - Main menu background
├── game_logo.png (400x150) - Game logo
├── button_normal.png (200x60) - Menu buttons
├── button_selected.png (200x60) - Selected menu buttons
├── slider_bg.png (200x20) - Settings sliders
├── slider_handle.png (20x20) - Slider handles
├── combine_button.png (80x30) - Weapon combine button
├── combine_arrow.png (32x16) - Combine indicator
├── combine_glow.png (64x64) - Combine effect
├── levelup_bg.png (600x400) - Level up menu background
├── upgrade_card.png (180x120) - Upgrade option cards
├── upgrade_card_selected.png (180x120) - Selected upgrade card
└── healthpack.png (24x24) - Health pickup
```

#### Weapon Sprites
```
assets/weapons/
├── railgun.png (48x48) - Railgun Tier 1
├── railgun2.png (48x48) - Railgun Tier 2+
├── shotgun.png (48x48) - Shotgun Tier 1
├── shotgun2.png (48x48) - Shotgun Tier 2
└── shotgun3.png (48x48) - Shotgun Tier 3+
```

#### Effect Sprites
```
assets/effects/
├── railgun_charge.png (32x32) - Railgun charging
├── heal_effect.png (32x32) - Healing effect
├── vampirism_effect.png (48x48) - Vampirism effect
└── muzzle_flash_shotgun.png (32x32) - Shotgun muzzle flash
```

#### Mobile Assets
```
assets/mobile/
├── joystick_bg.png (128x128) - Virtual joystick background
├── joystick_knob.png (64x64) - Joystick control knob
├── shoot_button.png (96x96) - Touch shoot button
└── menu_button.png (48x48) - Mobile menu button
```

#### Stat Icons (16x16 each)
```
assets/icons/
├── damage_icon.png
├── health_icon.png
├── speed_icon.png
├── armor_icon.png
├── regen_icon.png
├── vampirism_icon.png
├── crit_icon.png
├── luck_icon.png
├── range_icon.png
└── attack_speed_icon.png
```

### Audio Assets (Optional - if audio system is implemented)
```
assets/sounds/
├── railgun_charge.wav
├── railgun_fire.wav
├── shotgun_fire.wav
├── weapon_combine.wav
├── level_up.wav
├── heal_pickup.wav
└── menu_select.wav
```

---

## 🔄 Development Workflow

### Phase 1: Foundation (Week 1)
**Parallel Development - Simple Tasks**
- Mac build (TBD)
- Screenshot mode (Sasha Shukala)
- Stats on levelup (Artem)

### Phase 2: Core Features (Week 2)
**Parallel Development - Medium Complexity**
- Railgun (Sergey K)
- Shotgun (Viktor)
- Healing & Vampirism (Sergey Shurygin)

### Phase 3: Complex Systems (Week 3-4)
**Sequential Development - High Complexity**
- Weapon combining system (Viktor) - *Depends on Phase 2*
- Main menu with settings (Sasha Shukala)

### Phase 4: Advanced Features (Long-term)
**After Core Features Complete**
- Mobile platforms port (Sergey Shurygin) - *Depends on Main Menu*

---

## ✅ Testing & Validation

### Individual Task Testing
Each participant should test:
- [ ] Code compiles without warnings
- [ ] No regressions in existing functionality
- [ ] New features work as intended
- [ ] Performance impact is minimal
- [ ] Code follows existing project style

### Integration Testing
Before merging:
1. **Weapon Compatibility**: All new weapons work with shop system
2. **Game Balance**: New features don't break game difficulty
3. **UI Consistency**: All new UI follows existing design
4. **Save/Load**: Settings persist correctly between sessions

### Cross-Platform Testing
- [ ] Windows build works
- [ ] Mac build works (when implemented)
- [ ] Mobile build works (when implemented)

---

## 📞 Communication & Coordination

### Daily Standup Questions
1. What did you complete yesterday?
2. What are you working on today?
3. Are there any blockers or dependencies?
4. Do you need any assets or help from other team members?

### Asset Request Process
1. Check required resources section for your task
2. Create assets yourself or request from team
3. Use consistent naming convention
4. Test assets in-game before committing

### Code Review Guidelines
- Review code for style consistency
- Test functionality thoroughly
- Check for performance impacts
- Ensure documentation is updated

---

**Good luck with development! 🚀**

*This plan will be updated as development progresses and team feedback is received.*
