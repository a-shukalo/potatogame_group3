#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Core/Events/EventManager.h"
#include "../../Input/InputSystem/InputManager.h"
#include "../../Rendering/Renderer/IRenderer.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"
#include <vector>
#include <string>
#include <random>
#include <unordered_map>
#include <SDL2/SDL.h>

namespace BrotatoGame {
    namespace Gameplay {
        
        // Forward declarations
        class WaveSystem;
        
        enum class ShopState {
            CLOSED,    // Shop is not active
            OPENING,   // Shop is opening (transition)
            OPEN,      // Shop is open and active
            CLOSING    // Shop is closing (transition)
        };
        
        // Shop item type enumeration (from original game)
        enum class ShopItemType {
            WEAPON,
            ITEM  // For future items/upgrades
        };
        
        // Use WeaponTier from Components.h
        using WeaponTier = Gameplay::WeaponTier;
        
        // Shop item structure matching original game
        struct ShopItem {
            ShopItemType type;
            Core::WeaponType weaponType;
            WeaponTier tier;
            int price;
            bool locked;
            std::string name;
            std::string description;
            
            ShopItem(Core::WeaponType wType, WeaponTier wTier, int cost);
            
        private:
            std::string getWeaponName(Core::WeaponType wType, WeaponTier wTier);
            std::string getWeaponDescription(Core::WeaponType wType, WeaponTier wTier);
        };
        
        class ShopSystem : public Core::ISystem {
        public:
            ShopSystem();
            ~ShopSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void render() override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            void setWaveSystem(std::shared_ptr<WaveSystem> ws) { waveSystem = ws; }
            void setPlayerEntity(EntityId player) { playerEntity = player; }
            
            // Shop management  
            void openShop(int waveNumber);
            void closeShop();
            void generateItems(int waveNumber, int playerLuck = 0);
            
            // Shop state
            bool isShopActive() const { return shopState == ShopState::OPEN; }
            bool isShopOpening() const { return shopState == ShopState::OPENING || shopState == ShopState::OPEN; }
            ShopState getShopState() const { return shopState; }
            
            // Shop actions
            void buyItem(int index);
            void lockItem(int index); 
            void rerollItems();
            
            // Input and UI
            void handleInput(float deltaTime);
            void handleKeyboardState(const Uint8* keyState); // New method like original
            void handleMouseInput(int mouseX, int mouseY, bool mousePressed);
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<WaveSystem> waveSystem;
            std::shared_ptr<Core::EventManager> eventManager;
            std::shared_ptr<Input::InputManager> inputManager;
            std::shared_ptr<Rendering::IRenderer> renderer;
            EntityId playerEntity;
            
            // Weapon textures
            std::unordered_map<Core::WeaponType, std::string> weaponTexturePaths;
            std::unordered_map<Core::WeaponType, Rendering::TextureHandle> weaponTextures;
            
            // Shop state
            ShopState shopState;
            int currentWave;
            int rerollCount;
            std::vector<ShopItem> items;
            
            // UI state
            int hoveredItem;
            int selectedItem;
            bool lastMousePressed;
            
            // Constants
            static const int MAX_SHOP_ITEMS = 4;
            
            // Shop logic
            int calculateItemPrice(Core::WeaponType weaponType, WeaponTier tier, int waveNumber);
            int calculateRerollPrice(int waveNumber, int rerollCount);
            bool canItemAppear(Core::WeaponType weaponType, WeaponTier tier, int waveNumber);
            
            // UI rendering
            void renderShopUI();
            void renderShopItem(const ShopItem& item, int x, int y, int width, int height, bool highlighted, int index);
            void renderCharacterStats(int x, int y, int width, int height);
            
            // Shop lifecycle
            void onShopOpen();
            void onShopClose();
            
            // Input handling - track key states like original
            bool keyPressed[4]; // Track 1-4 key states to prevent multiple presses
            bool rPressed;      // For reroll  
            bool escapePressed; // For closing shop
            bool spacePressed;  // Alternative close
            
            // Random generation
            std::mt19937 randomGenerator;
            std::uniform_real_distribution<float> randomFloat;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
