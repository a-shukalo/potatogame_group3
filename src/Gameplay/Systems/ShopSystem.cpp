#include "ShopSystem.h"
#include "WaveSystem.h"
#include "../../Rendering/Renderer/IRenderer.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>
#include <algorithm>
#include <random>

namespace BrotatoGame {
    namespace Gameplay {
        
        // ShopItem implementation
        ShopItem::ShopItem(Core::WeaponType wType, WeaponTier wTier, int cost) 
            : type(ShopItemType::WEAPON), weaponType(wType), tier(wTier), 
              price(cost), locked(false) {
            name = getWeaponName(wType, wTier);
            description = getWeaponDescription(wType, wTier);
        }
        
        std::string ShopItem::getWeaponName(Core::WeaponType wType, WeaponTier wTier) {
            std::string baseName;
            switch (wType) {
                case Core::WeaponType::PISTOL: baseName = "Pistol"; break;
                case Core::WeaponType::SMG: baseName = "SMG"; break;
                case Core::WeaponType::SHOTGUN: baseName = "Shotgun"; break;
                case Core::WeaponType::RAILGUN: baseName = "Railgun"; break;
                case Core::WeaponType::MELEE_STICK: baseName = "Brick on Stick"; break;
                default: baseName = "Unknown"; break;
            }
            
            std::string tierName;
            switch (wTier) {
                case WeaponTier::TIER_1: tierName = ""; break;
                case WeaponTier::TIER_2: tierName = "Tier II "; break;
                case WeaponTier::TIER_3: tierName = "Tier III "; break;
                case WeaponTier::TIER_4: tierName = "Tier IV "; break;
            }
            
            return tierName + baseName;
        }
        
        std::string ShopItem::getWeaponDescription(Core::WeaponType wType, WeaponTier wTier) {
            switch (wType) {
                case Core::WeaponType::PISTOL:
                    return "Pierces 1 enemy, -50% damage to 2nd";
                case Core::WeaponType::SMG:
                    return "Fast fire rate, inaccurate shots";
                case Core::WeaponType::SHOTGUN:
                    return "Fires multiple pellets, close range";
                case Core::WeaponType::RAILGUN:
                    return "Charge weapon, pierces all enemies";
                case Core::WeaponType::MELEE_STICK:
                    return "Melee weapon, close combat";
            }
            return "";
        }
        
        // ShopSystem implementation
        ShopSystem::ShopSystem() 
            : shopState(ShopState::CLOSED), currentWave(0), rerollCount(0), hoveredItem(-1), selectedItem(0),
              lastMousePressed(false), escapePressed(false), spacePressed(false), rPressed(false),
              randomGenerator(std::random_device{}()), randomFloat(0.0f, 1.0f)
        {
            playerEntity = Core::INVALID_ENTITY_ID;
            
            // Initialize key tracking arrays  
            for (int i = 0; i < 4; i++) {
                keyPressed[i] = false;
            }
        }
        
        bool ShopSystem::initialize() {
            eventManager = Core::Services::getInstance().getService<Core::EventManager>();
            inputManager = Core::Services::getInstance().getService<Input::InputManager>();
            renderer = Core::Services::getInstance().getService<Rendering::IRenderer>();
            
            if (!eventManager || !inputManager || !renderer) {
                std::cout << "ShopSystem: Failed to get required services!" << std::endl;
                return false;
            }
            
            // Initialize weapon texture paths and load textures
            weaponTexturePaths[WeaponType::PISTOL] = "assets/weapons/pistol.png";
            weaponTexturePaths[WeaponType::SMG] = "assets/weapons/smg.png";
            weaponTexturePaths[WeaponType::SHOTGUN] = "assets/weapons/shotgun.png";
            weaponTexturePaths[WeaponType::RAILGUN] = "assets/weapons/sniper.png";
            weaponTexturePaths[WeaponType::MELEE_STICK] = "assets/weapons/brickonstick.png";
            
            // Load weapon textures
            for (const auto& [weaponType, path] : weaponTexturePaths) {
                auto textureHandle = renderer->loadTexture(path);
                if (textureHandle != 0) { // Assuming 0 is invalid texture handle
                    weaponTextures[weaponType] = textureHandle;
                    std::cout << "Loaded weapon texture: " << path << std::endl;
                } else {
                    std::cout << "Failed to load weapon texture: " << path << std::endl;
                }
            }
            
            std::cout << "ShopSystem initialized successfully!" << std::endl;
            return true;
        }
        
        void ShopSystem::update(float deltaTime) {
            if (shopState == ShopState::OPEN) {
                handleInput(deltaTime);
            }
        }
        
        void ShopSystem::render() {
            if (isShopActive()) {
                renderShopUI();
            }
        }
        
        void ShopSystem::shutdown() {
            // Unload weapon textures
            for (const auto& [weaponType, textureHandle] : weaponTextures) {
                if (renderer && textureHandle != 0) {
                    renderer->unloadTexture(textureHandle);
                }
            }
            weaponTextures.clear();
            
            std::cout << "ShopSystem shutdown." << std::endl;
        }
        
        void ShopSystem::openShop(int waveNumber) {
            if (shopState != ShopState::CLOSED) {
                std::cout << "ShopSystem: Cannot open shop - already open!" << std::endl;
                return;
            }
            
            shopState = ShopState::OPEN;
            currentWave = waveNumber;
            rerollCount = 0;
            selectedItem = 0;
            hoveredItem = -1;
            
            generateItems(waveNumber);
            onShopOpen();
            
            std::cout << "Shop opened after wave " << waveNumber << std::endl;
        }
        
        void ShopSystem::closeShop() {
            if (shopState != ShopState::OPEN) {
                return;
            }
            
            shopState = ShopState::CLOSED;
            items.clear();
            onShopClose();
            
            // Notify WaveSystem to start next wave
            if (waveSystem) {
                waveSystem->closeShop();
            }
            
            std::cout << "Shop closed." << std::endl;
        }
        
        void ShopSystem::generateItems(int waveNumber, int playerLuck) {
            items.clear();
            
            // Generate 4 random items like original Brotato
            for (int i = 0; i < MAX_SHOP_ITEMS; i++) {
                // Choose weapon type (for now just pistol and SMG, expand later)
                std::uniform_int_distribution<int> weaponDist(0, 1);
                Core::WeaponType weaponType = (weaponDist(randomGenerator) == 0) ? 
                    Core::WeaponType::PISTOL : Core::WeaponType::SMG;
                
                // Choose tier based on wave number (Brotato tier restrictions)
                std::vector<WeaponTier> availableTiers;
                availableTiers.push_back(WeaponTier::TIER_1);
                
                if (waveNumber >= 2) availableTiers.push_back(WeaponTier::TIER_2);
                if (waveNumber >= 4) availableTiers.push_back(WeaponTier::TIER_3);
                if (waveNumber >= 8) availableTiers.push_back(WeaponTier::TIER_4);
                
                std::uniform_int_distribution<int> tierDist(0, availableTiers.size() - 1);
                WeaponTier tier = availableTiers[tierDist(randomGenerator)];
                
                int price = calculateItemPrice(weaponType, tier, waveNumber);
                items.emplace_back(weaponType, tier, price);
            }
            
            std::cout << "Generated " << items.size() << " shop items for wave " << waveNumber << std::endl;
        }
        
        void ShopSystem::buyItem(int index) {
            if (index < 0 || index >= items.size()) {
                return;
            }
            
            auto playerComp = entityManager->getComponent<Player>(playerEntity);
            if (!playerComp) {
                return;
            }
            
            const ShopItem& item = items[index];
            
            // Check if player can afford
            if (playerComp->materials < item.price) {
                std::cout << "Cannot buy " << item.name << " - not enough materials!" << std::endl;
                return;
            }
            
            // Deduct materials
            playerComp->materials -= item.price;
            
            std::cout << "Bought " << item.name << " for " << item.price << " materials!" << std::endl;
            std::cout << "Remaining materials: " << playerComp->materials << std::endl;
            
            // Add weapon to player - upgrade stats (like in Brotato)
            switch (item.weaponType) {
                case Core::WeaponType::PISTOL:
                    playerComp->damage += 5; // +5 damage
                    break;
                case Core::WeaponType::SMG:
                    playerComp->attackSpeed *= 1.5f; // Faster attack speed
                    playerComp->damage += 2;
                    break;
                case Core::WeaponType::SHOTGUN:
                    playerComp->damage += 10;
                    playerComp->attackSpeed *= 0.7f; // Slower but powerful
                    break;
                case Core::WeaponType::RAILGUN:
                    playerComp->damage += 20;
                    playerComp->attackSpeed *= 0.5f; // Much slower but very powerful
                    break;
                case Core::WeaponType::MELEE_STICK:
                    playerComp->damage += 15;
                    break;
            }
            
            std::cout << "Weapon upgraded! New damage: " << playerComp->damage << ", Attack speed: " << playerComp->attackSpeed << std::endl;
            
            // Remove item from shop (like in original)
            items.erase(items.begin() + index);
        }
        
        void ShopSystem::lockItem(int index) {
            if (index >= 0 && index < items.size()) {
                items[index].locked = !items[index].locked;
                std::cout << "Item " << index << " " << (items[index].locked ? "locked" : "unlocked") << std::endl;
            }
        }
        
        void ShopSystem::rerollItems() {
            auto playerComp = entityManager->getComponent<Player>(playerEntity);
            if (!playerComp) {
                return;
            }
            
            int rerollPrice = calculateRerollPrice(currentWave, rerollCount);
            
            if (playerComp->materials < rerollPrice) {
                std::cout << "Cannot reroll - not enough materials! Need " << rerollPrice << std::endl;
                return;
            }
            
            // Deduct materials
            playerComp->materials -= rerollPrice;
            rerollCount++;
            
            // Save locked items
            std::vector<ShopItem> lockedItems;
            for (int i = 0; i < items.size(); i++) {
                if (items[i].locked) {
                    lockedItems.push_back(items[i]);
                }
            }
            
            // Regenerate items
            generateItems(currentWave);
            
            // Restore locked items
            int lockedIndex = 0;
            for (int i = 0; i < items.size() && lockedIndex < lockedItems.size(); i++) {
                if (lockedIndex < lockedItems.size()) {
                    items[i] = lockedItems[lockedIndex];
                    lockedIndex++;
                }
            }
            
            std::cout << "Rerolled shop for " << rerollPrice << " materials (reroll #" << rerollCount << ")" << std::endl;
        }
        
        void ShopSystem::handleInput(float deltaTime) {
            if (!inputManager) return;
            
            // Handle escape/space to close shop
            bool currentEscape = inputManager->isKeyPressed(Input::KeyCode::ESCAPE);
            if (currentEscape && !escapePressed) {
                std::cout << "Player pressed ESCAPE - closing shop!" << std::endl;
                closeShop();
            }
            escapePressed = currentEscape;
            
            bool currentSpace = inputManager->isKeyPressed(Input::KeyCode::SPACE);
            if (currentSpace && !spacePressed) {
                std::cout << "Player pressed SPACE - closing shop!" << std::endl;
                closeShop();
            }
            spacePressed = currentSpace;
            
            // Handle R for reroll
            bool currentR = inputManager->isKeyPressed(Input::KeyCode::R);
            if (currentR && !rPressed) {
                std::cout << "Player pressed R - rerolling shop!" << std::endl;
                rerollItems();
            }
            rPressed = currentR;
            
            // Handle number keys to buy items (1-4)
            if (inputManager->isKeyPressed(Input::KeyCode::NUM_1)) buyItem(0);
            if (inputManager->isKeyPressed(Input::KeyCode::NUM_2)) buyItem(1);
            if (inputManager->isKeyPressed(Input::KeyCode::NUM_3)) buyItem(2);
            if (inputManager->isKeyPressed(Input::KeyCode::NUM_4)) buyItem(3);
        }
        
        void ShopSystem::handleKeyboardState(const Uint8* keyState) {
            if (!isShopActive()) return;
            
            // Buy items (1-4 keys) - like original game
            for (int i = 0; i < 4; i++) {
                SDL_Scancode key = (SDL_Scancode)(SDL_SCANCODE_1 + i);
                bool isPressed = keyState[key];
                
                if (isPressed && !keyPressed[i]) {
                    keyPressed[i] = true;
                    if (i < static_cast<int>(items.size())) {
                        buyItem(i);
                    }
                } else if (!isPressed) {
                    keyPressed[i] = false;
                }
            }
            
            // Reroll (R key)
            bool rKeyPressed = keyState[SDL_SCANCODE_R];
            if (rKeyPressed && !rPressed) {
                rPressed = true;
                rerollItems();
            } else if (!rKeyPressed) {
                rPressed = false;
            }
            
            // Close shop (ESC key)  
            if (keyState[SDL_SCANCODE_ESCAPE] && !escapePressed) {
                escapePressed = true;
                closeShop();
            } else if (!keyState[SDL_SCANCODE_ESCAPE]) {
                escapePressed = false;
            }
            
            // Alternative close (SPACE key)
            if (keyState[SDL_SCANCODE_SPACE] && !spacePressed) {
                spacePressed = true;
                closeShop();
            } else if (!keyState[SDL_SCANCODE_SPACE]) {
                spacePressed = false;
            }
        }
        
        void ShopSystem::handleMouseInput(int mouseX, int mouseY, bool mousePressed) {
            if (!isShopActive()) return;
            
            hoveredItem = -1;
            
            // TODO: Implement mouse interaction with shop items
            // For now, just keyboard controls work
            
            lastMousePressed = mousePressed;
        }
        
        int ShopSystem::calculateItemPrice(Core::WeaponType weaponType, WeaponTier tier, int waveNumber) {
            // Base prices from original game
            int basePrice = 15; // Tier 1 base
            
            // Tier scaling
            switch (tier) {
                case WeaponTier::TIER_1: basePrice = 15; break;
                case WeaponTier::TIER_2: basePrice = 25; break;
                case WeaponTier::TIER_3: basePrice = 40; break;
                case WeaponTier::TIER_4: basePrice = 60; break;
            }
            
            // Weapon type modifiers
            switch (weaponType) {
                case Core::WeaponType::PISTOL: break; // No modifier
                case Core::WeaponType::SMG: basePrice += 5; break;
                case Core::WeaponType::SHOTGUN: basePrice += 10; break;
                case Core::WeaponType::RAILGUN: basePrice += 20; break;
                case Core::WeaponType::MELEE_STICK: basePrice -= 5; break;
            }
            
            return std::max(5, basePrice); // Minimum 5 materials
        }
        
        int ShopSystem::calculateRerollPrice(int waveNumber, int rerollCount) {
            // Base reroll price increases with each reroll and wave
            int basePrice = 2 + waveNumber;
            return basePrice + (rerollCount * 2);
        }
        
        bool ShopSystem::canItemAppear(Core::WeaponType weaponType, WeaponTier tier, int waveNumber) {
            // Tier restrictions based on wave number (like original)
            switch (tier) {
                case WeaponTier::TIER_1: return true;
                case WeaponTier::TIER_2: return waveNumber >= 2;
                case WeaponTier::TIER_3: return waveNumber >= 4;
                case WeaponTier::TIER_4: return waveNumber >= 8;
            }
            return false;
        }
        
        void ShopSystem::renderShopUI() {
            if (!renderer) return;
            
            // Shop background (dark overlay)
            Rendering::Color overlayColor(20, 22, 30, 240);
            renderer->clear(overlayColor);
            
            // Title
            renderer->drawText("WEAPON SHOP", Vector2(100, 50), Rendering::Color::white(), 24);
            
            // Materials display
            auto playerComp = entityManager->getComponent<Player>(playerEntity);
            if (playerComp) {
                std::string materialsText = "Materials: " + std::to_string(playerComp->materials);
                renderer->drawText(materialsText, Vector2(1500, 50), Rendering::Color::white(), 20);
            }
            
            // Instructions
            renderer->drawText("1-4: BUY ITEMS  |  R: REROLL  |  ESC/SPACE: CLOSE", 
                             Vector2(100, 100), Rendering::Color::white(), 16);
            
            // Shop items in horizontal row
            int itemWidth = 300;
            int itemHeight = 400;
            int spacing = 50;
            int startX = 200;
            int startY = 200;
            
            for (int i = 0; i < items.size() && i < MAX_SHOP_ITEMS; i++) {
                int x = startX + i * (itemWidth + spacing);
                int y = startY;
                bool highlighted = (i == hoveredItem) || (i == selectedItem);
                renderShopItem(items[i], x, y, itemWidth, itemHeight, highlighted, i);
            }
            
            // Reroll button info
            int rerollPrice = calculateRerollPrice(currentWave, rerollCount);
            std::string rerollText = "Reroll: " + std::to_string(rerollPrice) + " materials (Press R)";
            renderer->drawText(rerollText, Vector2(100, 800), Rendering::Color::yellow(), 18);
        }
        
        void ShopSystem::renderShopItem(const ShopItem& item, int x, int y, int width, int height, bool highlighted, int index) {
            if (!renderer) return;
            
            auto playerComp = entityManager->getComponent<Player>(playerEntity);
            bool canAfford = playerComp && playerComp->materials >= item.price;
            
            // Item card background
            Rendering::Color cardColor;
            if (item.locked) {
                cardColor = Rendering::Color(120, 100, 50, 255); // Locked color
            } else if (highlighted) {
                cardColor = canAfford ? Rendering::Color(80, 120, 160, 255) : Rendering::Color(60, 80, 100, 255);
            } else {
                cardColor = canAfford ? Rendering::Color(60, 70, 90, 255) : Rendering::Color(40, 50, 60, 255);
            }
            
            Rendering::Rectangle itemRect(x, y, width, height);
            renderer->drawRectangle(itemRect, cardColor, true);
            
            // Border
            Rendering::Color borderColor = highlighted ? Rendering::Color(200, 200, 255, 255) : Rendering::Color(120, 120, 140, 255);
            renderer->drawRectangle(itemRect, borderColor, false);
            
            // Weapon icon - centered at top of card (same size as original: ~40x40)
            auto textureIt = weaponTextures.find(item.weaponType);
            if (textureIt != weaponTextures.end()) {
                int iconSize = 40; // Same as original (wSlotSize - 16 = 56 - 16 = 40)
                Rendering::Rectangle destRect(x + width/2 - iconSize/2, y + 20, iconSize, iconSize);
                Rendering::Rectangle sourceRect(0, 0, 0, 0); // Let renderer handle full texture
                
                // Use the two-parameter version, but handle sourceRect properly
                if (renderer) {
                    // We'll create a simpler version - just use Transform for consistent sizing
                    Vector2 iconPos(x + width/2 - iconSize/2, y + 20);
                    Rendering::Transform iconTransform;
                    iconTransform.position = iconPos;
                    iconTransform.scale = Vector2(1.0f, 1.0f); // Normal scale, let texture size determine final size
                    iconTransform.rotation = 0.0f;
                    
                    renderer->drawTexture(textureIt->second, iconTransform);
                }
            }
            
            // Item name - below icon
            Vector2 namePos(x + width/2, y + 90);
            renderer->drawText(item.name, namePos, Rendering::Color::white(), 16);
            
            // Item description - below name
            Vector2 descPos(x + 10, y + 120);
            renderer->drawText(item.description, descPos, Rendering::Color(200, 200, 200, 255), 12);
            
            // Price and buy button
            std::string priceText = "Price: " + std::to_string(item.price);
            renderer->drawText(priceText, Vector2(x + 10, y + height - 80), Rendering::Color::white(), 14);
            
            std::string buyText = "Press " + std::to_string(index + 1);
            renderer->drawText(buyText, Vector2(x + 10, y + height - 50), Rendering::Color::yellow(), 12);
            
            // Locked indicator
            if (item.locked) {
                renderer->drawText("LOCKED", Vector2(x + width - 80, y + 10), Rendering::Color::red(), 14);
            }
        }
        
        void ShopSystem::renderCharacterStats(int x, int y, int width, int height) {
            if (!renderer) return;
            
            // Character stats panel (placeholder for now)
            Rendering::Rectangle statsRect(x, y, width, height);
            renderer->drawRectangle(statsRect, Rendering::Color(40, 45, 55, 255), true);
            renderer->drawRectangle(statsRect, Rendering::Color(100, 110, 130, 255), false);
            
            renderer->drawText("PLAYER STATS", Vector2(x + 10, y + 10), Rendering::Color::white(), 16);
            renderer->drawText("(TODO: Implement)", Vector2(x + 10, y + 40), Rendering::Color(150, 150, 150, 255), 12);
        }
        
        void ShopSystem::onShopOpen() {
            std::cout << "=== SHOP OPENED ===" << std::endl;
            std::cout << "Wave " << currentWave << " completed!" << std::endl;
            std::cout << "Items available:" << std::endl;
            for (int i = 0; i < items.size(); i++) {
                std::cout << "  " << (i+1) << ". " << items[i].name << " - " << items[i].price << " materials" << std::endl;
            }
        }
        
        void ShopSystem::onShopClose() {
            std::cout << "=== SHOP CLOSED ===" << std::endl;
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame