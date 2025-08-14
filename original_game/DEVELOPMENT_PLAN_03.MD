# План разработки - Этап 03
## Расширенные функции игры Brotato MVP

**Версия документа**: 1.0  
**Дата создания**: Декабрь 2024  
**Команда разработки**: 6 участников

---

## 📋 Обзор задач

| № | Задача | Исполнитель | Приоритет | Сложность | Время |
|---|--------|-------------|-----------|-----------|-------|
| 0 | Mac build | TBD | Высокий | Средняя | 2-3 дня |
| 1 | Screenshot mode | Sasha Shukala | Низкий | Легкая | 1 день |
| 2 | Railgun | Сергей К | Средний | Средняя | 2-3 дня |
| 2.1 | Shotgun | Viktor | Средний | Средняя | 2-3 дня |
| 3 | Weapon upgrading/combining | Viktor | Высокий | Сложная | 4-5 дней |
| 4 | Healing items, vampirism | Sergey Shurygin | Средний | Средняя | 3-4 дня |
| 5 | Main menu with settings | Sasha Shukala | Высокий | Сложная | 4-5 дней |
| 6 | Mobile platforms port | Sergey Shurygin | Низкий | Очень сложная | 7-10 дней |
| 7 | Improve stats on levelup | Artem | Средний | Легкая | 1-2 дня |

---

## 🎯 Задача 0: Mac Build

### Описание
Адаптация проекта для сборки и запуска на macOS с использованием Xcode и Homebrew.

### Технические требования
- **Система сборки**: CMake + vcpkg/Homebrew
- **Компилятор**: Clang (Apple)
- **Зависимости**: SDL2, SDL2_image, SDL2_ttf через Homebrew

### Файлы для изменения
```
CMakeLists.txt (адаптация для macOS)
vcpkg.json (добавить macOS triplet)
build-mac.sh (новый скрипт сборки)
README.md (обновить инструкции)
```

### Детальный план реализации

#### Шаг 1: Настройка среды сборки (0.5 дня)
1. **Создать скрипт установки зависимостей**
   ```bash
   # install-deps-mac.sh
   brew install sdl2 sdl2_image sdl2_ttf cmake
   ```

2. **Обновить CMakeLists.txt**
   ```cmake
   # Добавить проверку платформы
   if(APPLE)
       set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
       find_package(PkgConfig REQUIRED)
       pkg_check_modules(SDL2 REQUIRED sdl2)
       pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
       pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
   endif()
   ```

#### Шаг 2: Адаптация кода (1 день)
1. **Проверить совместимость заголовков**
   - Убедиться в корректных путях включения SDL2
   - Проверить использование Windows-специфичных функций

2. **Обновить пути к ресурсам**
   ```cpp
   // Использовать относительные пути или bundle resources
   #ifdef __APPLE__
       std::string getResourcePath(const std::string& resource);
   #endif
   ```

#### Шаг 3: Тестирование и отладка (1-1.5 дня)
1. **Создать CI/CD для macOS** (GitHub Actions)
2. **Тестировать на различных версиях macOS**
3. **Создать .app bundle для распространения**

### Критерии приемки
- [ ] Проект собирается на macOS без ошибок
- [ ] Игра запускается и работает корректно
- [ ] Все ресурсы загружаются правильно
- [ ] Создан установочный пакет .dmg

---

## 📸 Задача 1: Screenshot Mode (Sasha Shukala)

### Описание
Режим скриншота, который скрывает весь UI для создания чистых снимков игрового процесса.

### Технические требования
- **Горячая клавиша**: F12 или Print Screen
- **Функциональность**: Полное скрытие UI с сохранением геймплея
- **Индикатор**: Небольшая иконка в углу экрана

### Файлы для изменения
```
src/Game.h (добавить флаг screenshotMode)
src/Game.cpp (обработка ввода и рендеринг)
```

### Детальный план реализации

#### Шаг 1: Добавление флага состояния (0.2 дня)
```cpp
// В Game.h
class Game {
private:
    bool screenshotMode;
    bool screenshotKeyPressed;
    
public:
    void toggleScreenshotMode();
    bool isScreenshotMode() const { return screenshotMode; }
};
```

#### Шаг 2: Обработка ввода (0.3 дня)
```cpp
// В Game.cpp - handleEvents()
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

#### Шаг 3: Условный рендеринг UI (0.5 дня)
```cpp
// В Game.cpp - render()
void Game::render() {
    // ... рендер игровых объектов ...
    
    if (!screenshotMode) {
        renderUI();
    } else {
        // Показать небольшую иконку камеры
        renderScreenshotIndicator();
    }
}
```

### Критерии приемки
- [ ] F12 переключает режим скриншота
- [ ] В режиме скриншота весь UI скрыт
- [ ] Геймплей продолжается нормально
- [ ] Есть визуальный индикатор активного режима
- [ ] Повторное нажатие F12 восстанавливает UI

---

## 🔫 Задача 2: Railgun (Сергей К)

### Описание
Высокотехнологичное оружие с мгновенным проникающим выстрелом через всех врагов на линии.

### Технические требования
- **Тип урона**: Высокий разовый урон
- **Особенность**: Проникает через всех врагов на линии
- **Эффекты**: Лазерный луч, электрические искры
- **Перезарядка**: Длительная (2-3 секунды)

### Файлы для изменения
```
src/Weapon.h (добавить WeaponType::RAILGUN)
src/Weapon.cpp (реализация railgun логики)
src/Bullet.h (добавить BulletType::RAILGUN_BEAM)
src/Bullet.cpp (проникающий урон)
src/Game.cpp (специальная обработка railgun пуль)
assets/weapons/railgun.png (новый спрайт)
```

### Детальный план реализации

#### Шаг 1: Расширение системы оружия (1 день)
```cpp
// В Weapon.h
enum class WeaponType {
    PISTOL,
    SMG,
    MELEE_STICK,
    RAILGUN  // Новый тип
};

// В Weapon.cpp
void Weapon::initializeRailgunStats() {
    stats.baseDamage = 80;      // Высокий урон
    stats.attackSpeed = 2.5f;   // Медленная перезарядка
    stats.range = 2000;         // Через весь экран
    stats.critChance = 0.25f;   // Высокий шанс крита
    stats.critMultiplier = 3.0f;
}
```

#### Шаг 2: Специальные пули (1 день)
```cpp
// В Bullet.h
enum class BulletType {
    NORMAL,
    RAILGUN_BEAM
};

class Bullet {
private:
    BulletType bulletType;
    int remainingPierces;
    
public:
    bool canPierce() const;
    void onHitEnemy();
};
```

#### Шаг 3: Визуальные эффекты (0.5-1 день)
```cpp
// В Game.cpp
void Game::renderRailgunBeam(const Vector2& start, const Vector2& end) {
    // Рендер лазерного луча
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Cyan
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
    
    // Добавить эффект свечения
    for (int i = 1; i <= 3; i++) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 128/i);
        // Рендер дополнительных линий для эффекта свечения
    }
}
```

### Критерии приемки
- [ ] Railgun доступен в магазине (Tier 3-4)
- [ ] Мгновенный выстрел через всех врагов на линии
- [ ] Высокий урон с длительной перезарядкой
- [ ] Визуальный эффект лазерного луча
- [ ] Звуковой эффект выстрела (если добавлен звук)

---

## 💥 Задача 2.1: Shotgun (Viktor)

### Описание
Дробовик с разбросом пуль на короткой дистанции, эффективен против групп врагов.

### Технические требования
- **Паттерн стрельбы**: 5-7 пуль в конусе
- **Урон**: Средний за пулю, высокий суммарный
- **Дальность**: Ограниченная
- **Разброс**: 15-30 градусов

### Файлы для изменения
```
src/Weapon.h (WeaponType::SHOTGUN)
src/Weapon.cpp (логика разброса)
src/Bullet.cpp (короткоживущие пули)
assets/weapons/shotgun.png
assets/weapons/shotgun2.png
```

### Детальный план реализации

#### Шаг 1: Добавление типа оружия (0.5 дня)
```cpp
// В Weapon.h
enum class WeaponType {
    PISTOL,
    SMG,
    MELEE_STICK,
    RAILGUN,
    SHOTGUN  // Новый тип
};
```

#### Шаг 2: Реализация логики разброса (1.5 дня)
```cpp
// В Weapon.cpp
void Weapon::fireShotgun(const Vector2& weaponPos, const Vector2& direction, 
                        std::vector<std::unique_ptr<Bullet>>& bullets,
                        const Player& player) {
    const int pelletCount = 6;
    const float spreadAngle = 25.0f; // градусы
    
    for (int i = 0; i < pelletCount; i++) {
        float angleOffset = (i - pelletCount/2.0f) * (spreadAngle / pelletCount);
        Vector2 pelletDirection = rotateVector(direction, angleOffset);
        
        auto bullet = std::make_unique<Bullet>(weaponPos, pelletDirection, 
                                             calculateDamage(player) / 2);
        bullet->setLifetime(0.3f); // Короткая дальность
        bullets.push_back(std::move(bullet));
    }
}
```

#### Шаг 3: Балансировка и эффекты (1 день)
```cpp
void Weapon::initializeShotgunStats() {
    switch (tier) {
        case WeaponTier::TIER_1:
            stats.baseDamage = 8;       // За пулю
            stats.attackSpeed = 1.8f;   // Медленнее пистолета
            stats.range = 150;          // Короткая дальность
            break;
        case WeaponTier::TIER_2:
            stats.baseDamage = 12;
            stats.attackSpeed = 1.6f;
            stats.range = 200;
            break;
        // ... остальные тиры
    }
}
```

### Критерии приемки
- [ ] Shotgun стреляет несколькими пулями с разбросом
- [ ] Эффективен на короткой дистанции
- [ ] Урон падает с расстоянием
- [ ] Доступен в магазине в разных тирах
- [ ] Визуальный эффект дульной вспышки

---

## ⚒️ Задача 3: Weapon Upgrading/Combining System (Viktor)

### Описание
Система улучшения и комбинирования оружия для создания более мощных вариантов.

### Технические требования
- **Механика**: Объединение 2-3 одинаковых оружий в улучшенную версию
- **Интерфейс**: Drag & Drop в инвентаре или автоматическое объединение
- **Эффекты**: Визуальные индикаторы возможности объединения

### Файлы для изменения
```
src/Player.h (система инвентаря)
src/Player.cpp (логика объединения)
src/Shop.h (UI для объединения)
src/Shop.cpp (интерфейс объединения)
src/Weapon.h (система тиров и комбинирования)
src/Game.cpp (обработка событий объединения)
```

### Детальный план реализации

#### Шаг 1: Расширение системы инвентаря (1.5 дня)
```cpp
// В Player.h
class Player {
private:
    struct WeaponSlot {
        std::unique_ptr<Weapon> weapon;
        bool canCombine;
        int combineCount; // Сколько оружий этого типа есть
    };
    
    std::vector<WeaponSlot> weaponSlots;
    
public:
    bool canCombineWeapons(WeaponType type, WeaponTier tier) const;
    std::unique_ptr<Weapon> combineWeapons(WeaponType type, WeaponTier tier);
    void updateCombinePossibilities();
};
```

#### Шаг 2: Логика комбинирования (2 дня)
```cpp
// В Player.cpp
bool Player::canCombineWeapons(WeaponType type, WeaponTier tier) const {
    int count = 0;
    for (const auto& slot : weaponSlots) {
        if (slot.weapon && 
            slot.weapon->getType() == type && 
            slot.weapon->getTier() == tier) {
            count++;
        }
    }
    return count >= 3; // Нужно 3 оружия для объединения
}

std::unique_ptr<Weapon> Player::combineWeapons(WeaponType type, WeaponTier tier) {
    // Удаляем 3 оружия одного типа
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
    
    // Создаем улучшенную версию
    WeaponTier newTier = static_cast<WeaponTier>(static_cast<int>(tier) + 1);
    return std::make_unique<Weapon>(type, newTier);
}
```

#### Шаг 3: UI интеграция (1.5 дня)
```cpp
// В Shop.cpp
void Shop::renderWeaponCombining(SDL_Renderer* renderer, int x, int y) {
    // Показать доступные комбинации
    for (auto type : {WeaponType::PISTOL, WeaponType::SMG, WeaponType::SHOTGUN}) {
        for (auto tier : {WeaponTier::TIER_1, WeaponTier::TIER_2, WeaponTier::TIER_3}) {
            if (gameRef->getPlayer()->canCombineWeapons(type, tier)) {
                renderCombineButton(renderer, type, tier, x, y);
                y += 60;
            }
        }
    }
}
```

### Критерии приемки
- [ ] 3 оружия одного типа и тира можно объединить
- [ ] Объединение создает оружие следующего тира
- [ ] UI показывает возможные комбинации
- [ ] Есть кнопка/горячая клавиша для объединения
- [ ] Визуальные эффекты при объединении

---

## ❤️ Задача 4: Healing Items & Vampirism (Sergey Shurygin)

### Описание
Система лечения включающая предметы восстановления здоровья и вампиризм (восстановление здоровья при убийстве врагов).

### Технические требования
- **Предметы лечения**: Аптечки, выпадающие от врагов
- **Вампиризм**: Характеристика игрока, восстанавливающая здоровье при убийстве
- **Визуализация**: Эффекты лечения, индикаторы вампиризма

### Файлы для изменения
```
src/Player.h (добавить vampirism stat)
src/Player.cpp (логика лечения)
src/Game.cpp (спавн аптечек, обработка вампиризма)
src/HealthPickup.h (новый класс)
src/HealthPickup.cpp (логика аптечек)
assets/ui/healthpack.png (новый спрайт)
```

### Детальный план реализации

#### Шаг 1: Расширение системы характеристик (0.5 дня)
```cpp
// В Player.h
struct PlayerStats {
    // ... существующие характеристики ...
    float vampirism;        // 0.0 - 1.0 (процент восстановления от урона)
    float healingBonus;     // Множитель эффективности лечения
    
    PlayerStats() : /* ... */ vampirism(0.0f), healingBonus(1.0f) {}
};
```

#### Шаг 2: Класс аптечек (1 день)
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
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    
    Vector2 getPosition() const { return position; }
    int getHealAmount() const { return healAmount; }
    bool shouldDestroy() const { return lifetime <= 0; }
};
```

#### Шаг 3: Логика вампиризма (1 день)
```cpp
// В Player.cpp
void Player::onEnemyKilled(int damageDealt) {
    if (stats.vampirism > 0) {
        int healAmount = static_cast<int>(damageDealt * stats.vampirism);
        heal(healAmount);
        
        // Визуальный эффект вампиризма
        showVampirismEffect();
    }
}

void Player::heal(int amount) {
    int actualHeal = static_cast<int>(amount * stats.healingBonus);
    health = std::min(health + actualHeal, stats.maxHealth);
    
    // Эффект лечения
    showHealingEffect(actualHeal);
}
```

#### Шаг 4: Интеграция с игровым циклом (1 день)
```cpp
// В Game.cpp
void Game::spawnHealthPickups() {
    // 5% шанс выпадения аптечки при убийстве врага
    if (rand() % 100 < 5) {
        int healAmount = 15 + (wave * 2); // Больше лечения на поздних волнах
        healthPickups.push_back(
            std::make_unique<HealthPickup>(enemyPosition, healAmount)
        );
    }
}

void Game::updateHealthPickups() {
    for (auto it = healthPickups.begin(); it != healthPickups.end();) {
        (*it)->update(deltaTime);
        
        // Проверка подбора игроком
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

#### Шаг 5: Магазинные предметы (0.5 дня)
```cpp
// В Shop.cpp - добавить предметы вампиризма
struct VampirismItem : public ShopItem {
    VampirismItem() : ShopItem(ShopItemType::STAT_UPGRADE) {
        name = "Vampirism";
        description = "+10% lifesteal on kill";
        price = 25;
        statBonus.vampirism = 0.1f;
    }
};
```

### Критерии приемки
- [ ] Аптечки выпадают от убитых врагов
- [ ] Аптечки восстанавливают здоровье при подборе
- [ ] Вампиризм работает при убийстве врагов
- [ ] Визуальные эффекты лечения
- [ ] Предметы вампиризма доступны в магазине
- [ ] Лечение не превышает максимальное здоровье

---

## 🎮 Задача 5: Main Menu with Settings (Sasha Shukala)

### Описание
Главное меню игры с настройками графики, звука и управления.

### Технические требования
- **Меню**: Start Game, Settings, Exit
- **Настройки**: Volume, Resolution, Controls
- **Сохранение**: Настройки сохраняются между сессиями

### Файлы для изменения
```
src/GameState.h (новый файл - состояния игры)
src/MainMenu.h (новый файл)
src/MainMenu.cpp (новый файл)
src/Settings.h (новый файл)
src/Settings.cpp (новый файл)
src/Game.h (интеграция состояний)
src/Game.cpp (обновление игрового цикла)
assets/ui/menu_bg.png (фон меню)
```

### Детальный план реализации

#### Шаг 1: Система состояний игры (1 день)
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
    GameState currentState;
    GameState previousState;
    
public:
    void changeState(GameState newState);
    GameState getCurrentState() const { return currentState; }
    void returnToPrevious();
};
```

#### Шаг 2: Главное меню (1.5 дня)
```cpp
// MainMenu.h
class MainMenu {
private:
    SDL_Texture* backgroundTexture;
    SDL_Texture* logoTexture;
    
    enum class MenuOption {
        START_GAME,
        SETTINGS,
        EXIT
    };
    
    MenuOption selectedOption;
    bool keyPressed[3]; // Для обработки ввода
    
public:
    void render(SDL_Renderer* renderer);
    void handleInput(const Uint8* keyState);
    MenuOption getSelectedOption() const;
};
```

#### Шаг 3: Система настроек (2 дня)
```cpp
// Settings.h
struct GameSettings {
    float masterVolume;
    float sfxVolume;
    int windowWidth;
    int windowHeight;
    bool fullscreen;
    
    // Controls
    SDL_Scancode moveUp;
    SDL_Scancode moveDown;
    SDL_Scancode moveLeft;
    SDL_Scancode moveRight;
    SDL_Scancode shoot;
    
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void setDefaults();
};

class SettingsMenu {
private:
    GameSettings* settings;
    int selectedOption;
    bool editingKey;
    
public:
    void render(SDL_Renderer* renderer);
    void handleInput(const Uint8* keyState);
    bool isDone() const;
};
```

#### Шаг 4: Интеграция с основной игрой (1 день)
```cpp
// В Game.cpp
void Game::run() {
    while (running) {
        handleEvents();
        
        switch (stateManager.getCurrentState()) {
            case GameState::MAIN_MENU:
                mainMenu.handleInput(keyState);
                if (mainMenu.getSelectedOption() == MenuOption::START_GAME) {
                    stateManager.changeState(GameState::IN_GAME);
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

#### Шаг 5: Сохранение настроек (0.5 дня)
```cpp
// В Settings.cpp
void GameSettings::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "masterVolume=" << masterVolume << std::endl;
        file << "sfxVolume=" << sfxVolume << std::endl;
        file << "windowWidth=" << windowWidth << std::endl;
        file << "windowHeight=" << windowHeight << std::endl;
        file << "fullscreen=" << (fullscreen ? 1 : 0) << std::endl;
        // ... остальные настройки
    }
}
```

### Критерии приемки
- [ ] Игра запускается с главного меню
- [ ] Кнопки "Start Game", "Settings", "Exit" работают
- [ ] Меню настроек позволяет изменить громкость, разрешение
- [ ] Настройки сохраняются и загружаются
- [ ] ESC возвращает в предыдущее меню
- [ ] Навигация клавиатурой и мышью

---

## 📱 Задача 6: Mobile Platforms Port (Sergey Shurygin)

### Описание
Портирование игры на мобильные платформы с сенсорным управлением (приоритет Android).

### Технические требования
- **Платформы**: Android (приоритет), iOS (опционально)
- **Управление**: Виртуальный джойстик, сенсорная стрельба
- **Адаптация UI**: Масштабирование для различных экранов

### Файлы для изменения
```
CMakeLists.txt (Android toolchain)
src/InputManager.h (новый файл - абстракция ввода)
src/InputManager.cpp (обработка touch событий)
src/TouchControls.h (новый файл)
src/TouchControls.cpp (виртуальные элементы управления)
src/Game.cpp (адаптация для мобильных)
android/ (новая папка с Android проектом)
```

### Детальный план реализации

#### Шаг 1: Настройка Android SDK/NDK (1 день)
```bash
# Установка Android SDK, NDK, CMake
# Создание Android проекта с native activity
# Настройка gradle build system
```

#### Шаг 2: Абстракция системы ввода (2 дня)
```cpp
// InputManager.h
class InputManager {
public:
    virtual ~InputManager() = default;
    virtual Vector2 getMovementInput() = 0;
    virtual Vector2 getAimDirection() = 0;
    virtual bool isShootPressed() = 0;
    virtual bool isMenuPressed() = 0;
};

class DesktopInputManager : public InputManager {
    // Реализация для PC
};

class MobileInputManager : public InputManager {
    // Реализация для мобильных
};
```

#### Шаг 3: Сенсорные элементы управления (2-3 дня)
```cpp
// TouchControls.h
class TouchControls {
private:
    struct TouchButton {
        SDL_Rect bounds;
        bool pressed;
        SDL_Texture* texture;
    };
    
    TouchButton moveJoystick;
    TouchButton shootButton;
    Vector2 joystickCenter;
    Vector2 joystickCurrent;
    
public:
    void handleTouchEvent(SDL_TouchFingerEvent& event);
    void render(SDL_Renderer* renderer);
    Vector2 getMovementVector() const;
    bool isShootPressed() const;
};
```

#### Шаг 4: Адаптация UI для мобильных (2 дня)
```cpp
// В Game.cpp
void Game::adaptForMobile() {
    // Увеличить размеры UI элементов
    uiScale = 1.5f;
    
    // Переместить элементы для удобства
    healthBarPosition = Vector2(50, 100);
    
    // Добавить сенсорные элементы
    touchControls = std::make_unique<TouchControls>();
}
```

#### Шаг 5: Сборка и тестирование (2-3 дня)
```cmake
# Android CMakeLists.txt
cmake_minimum_required(VERSION 3.18)
project(BrotatoAndroid)

find_library(log-lib log)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image)

target_link_libraries(BrotatoAndroid
    ${log-lib}
    SDL2::SDL2
    SDL2_image::SDL2_image
)
```

### Критерии приемки
- [ ] APK собирается без ошибок
- [ ] Игра запускается на Android устройстве
- [ ] Виртуальный джойстик работает корректно
- [ ] Сенсорная стрельба функциональна
- [ ] UI адаптирован для мобильных экранов
- [ ] Производительность приемлемая (30+ FPS)

---

## 📊 Задача 7: Improve Stats on Levelup (Artem)

### Описание
Улучшение системы прокачки с выбором бонусов при повышении уровня.

### Технические требования
- **Выбор бонусов**: 3 случайных улучшения на выбор
- **Разнообразие**: Различные типы бонусов (урон, скорость, здоровье, etc.)
- **Баланс**: Сбалансированные улучшения для разных стилей игры

### Файлы для изменения
```
src/LevelUpMenu.h (новый файл)
src/LevelUpMenu.cpp (меню выбора улучшений)
src/Player.h (расширение системы характеристик)
src/Player.cpp (применение бонусов)
src/Game.cpp (пауза игры при level up)
```

### Детальный план реализации

#### Шаг 1: Система улучшений (1 день)
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
    int selectedOption;
    bool isActive;
    
public:
    void generateRandomUpgrades(int playerLevel);
    void render(SDL_Renderer* renderer);
    void handleInput(const Uint8* keyState);
    StatUpgrade getSelectedUpgrade();
    bool isMenuActive() const { return isActive; }
};
```

#### Шаг 2: Генерация случайных улучшений (0.5 дня)
```cpp
// В LevelUpMenu.cpp
void LevelUpMenu::generateRandomUpgrades(int playerLevel) {
    availableUpgrades.clear();
    
    // Пул всех возможных улучшений
    std::vector<StatUpgrade> upgradePool = {
        {StatUpgradeType::DAMAGE, "Damage +", "Increase weapon damage", 5.0f},
        {StatUpgradeType::MAX_HEALTH, "Max Health +", "Increase maximum health", 20.0f},
        {StatUpgradeType::MOVE_SPEED, "Speed +", "Move faster", 15.0f},
        // ... остальные улучшения
    };
    
    // Выбираем 3 случайных улучшения
    std::shuffle(upgradePool.begin(), upgradePool.end(), randomGenerator);
    for (int i = 0; i < 3 && i < upgradePool.size(); i++) {
        availableUpgrades.push_back(upgradePool[i]);
    }
}
```

#### Шаг 3: Применение улучшений (0.5 дня)
```cpp
// В LevelUpMenu.cpp
void StatUpgrade::applyToPlayer(Player& player) {
    PlayerStats& stats = player.getStats();
    
    switch (type) {
        case StatUpgradeType::DAMAGE:
            stats.damage += static_cast<int>(value);
            break;
        case StatUpgradeType::MAX_HEALTH:
            stats.maxHealth += static_cast<int>(value);
            player.heal(static_cast<int>(value)); // Также лечим
            break;
        case StatUpgradeType::MOVE_SPEED:
            stats.moveSpeed += value;
            break;
        // ... остальные случаи
    }
}
```

### Критерии приемки
- [ ] При level up игра паузится и показывается меню выбора
- [ ] Предлагается 3 случайных улучшения
- [ ] Выбранное улучшение применяется к игроку
- [ ] Улучшения сбалансированы и разнообразны
- [ ] UI меню понятен и удобен

---

## 🔄 Порядок реализации и зависимости

### Фаза 1 (Параллельная разработка - Неделя 1)
- **Задача 0**: Mac build (TBD) - **Приоритет: Высокий**
- **Задача 1**: Screenshot mode (Sasha Shukala) - **Простая задача**
- **Задача 7**: Improve stats on levelup (Artem) - **Простая задача**

### Фаза 2 (Параллельная разработка - Неделя 2)
- **Задача 2**: Railgun (Сергей К)
- **Задача 2.1**: Shotgun (Viktor) 
- **Задача 4**: Healing items, vampirism (Sergey Shurygin)

### Фаза 3 (Сложные системы - Неделя 3-4)
- **Задача 3**: Weapon upgrading/combining system (Viktor) - **Зависит от 2 и 2.1**
- **Задача 5**: Main menu with settings (Sasha Shukala)

### Фаза 4 (Долгосрочная - После основных функций)
- **Задача 6**: Mobile platforms port (Sergey Shurygin) - **Зависит от 5**

---

## 🧪 Тестирование и валидация

### Общие критерии для всех задач
- [ ] Код компилируется без предупреждений
- [ ] Нет регрессий в существующем функционале
- [ ] Новые функции работают как задумано
- [ ] Производительность не снижается значительно
- [ ] Код следует существующему стилю проекта

### Интеграционные тесты
1. **Совместимость оружия**: Все новые оружия работают с системой магазина
2. **Баланс игры**: Новые функции не делают игру слишком легкой/сложной
3. **UI консистентность**: Все новые UI элементы следуют общему стилю
4. **Сохранение/загрузка**: Настройки корректно сохраняются между сессиями

---

## 📚 Ресурсы и документация

### Полезные ссылки
- [SDL2 Documentation](https://wiki.libsdl.org/)
- [CMake Android Documentation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling-for-android)
- [Brotato Game Reference](https://store.steampowered.com/app/1942280/Brotato/)

### Файлы для изучения перед началом
- `src/Game.h` и `src/Game.cpp` - основной игровой цикл
- `src/Weapon.h` и `src/Weapon.cpp` - система оружия
- `src/Player.h` и `src/Player.cpp` - логика игрока
- `src/Shop.h` и `src/Shop.cpp` - система магазина

---

**Удачи в разработке! 🚀**

*Этот план будет обновляться по мере продвижения разработки и получения обратной связи от команды.*

