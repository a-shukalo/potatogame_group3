#include "GameplayManager.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>

namespace BrotatoGame {
    namespace Gameplay {
        
        GameplayManager::GameplayManager() : playerId(Core::INVALID_ENTITY_ID), gameTime(0.0f) {}
        
        bool GameplayManager::initialize() {
            // Create entity manager
            entityManager = std::make_shared<EntityManager>();
            
            // Create and initialize systems
            renderSystem = std::make_shared<RenderSystem>();
            movementSystem = std::make_shared<MovementSystem>();
            enemySpawnSystem = std::make_shared<EnemySpawnSystem>();
            enemyAISystem = std::make_shared<EnemyAISystem>();
            projectileSystem = std::make_shared<ProjectileSystem>();
            shootingSystem = std::make_shared<ShootingSystem>();
            collisionSystem = std::make_shared<CollisionSystem>();
            waveSystem = std::make_shared<WaveSystem>();
            shopSystem = std::make_shared<ShopSystem>();
            pickupSystem = std::make_shared<PickupSystem>();
            uiSystem = std::make_shared<UISystem>();
            
            if (!renderSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize render system!" << std::endl;
                return false;
            }
            
            if (!movementSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize movement system!" << std::endl;
                return false;
            }
            
            if (!enemySpawnSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize enemy spawn system!" << std::endl;
                return false;
            }
            
            if (!enemyAISystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize enemy AI system!" << std::endl;
                return false;
            }
            
            if (!projectileSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize projectile system!" << std::endl;
                return false;
            }
            
            if (!shootingSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize shooting system!" << std::endl;
                return false;
            }
            
            if (!collisionSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize collision system!" << std::endl;
                return false;
            }
            
            if (!waveSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize wave system!" << std::endl;
                return false;
            }
            
            if (!shopSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize shop system!" << std::endl;
                return false;
            }
            
            if (!pickupSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize pickup system!" << std::endl;
                return false;
            }
            
            if (!uiSystem->initialize()) {
                std::cout << "GameplayManager: Failed to initialize UI system!" << std::endl;
                return false;
            }
            
            // Set entity manager for systems
            renderSystem->setEntityManager(entityManager);
            movementSystem->setEntityManager(entityManager);
            enemySpawnSystem->setEntityManager(entityManager);
            enemyAISystem->setEntityManager(entityManager);
            projectileSystem->setEntityManager(entityManager);
            shootingSystem->setEntityManager(entityManager);
            collisionSystem->setEntityManager(entityManager);
            waveSystem->setEntityManager(entityManager);
            shopSystem->setEntityManager(entityManager);
            pickupSystem->setEntityManager(entityManager);
            uiSystem->setEntityManager(entityManager);
            
            // Connect systems that need references to each other
            shootingSystem->setProjectileSystem(projectileSystem);
            enemySpawnSystem->setWaveSystem(waveSystem);
            shopSystem->setWaveSystem(waveSystem);
            collisionSystem->setPickupSystem(pickupSystem);
            collisionSystem->setWaveSystem(waveSystem);
            uiSystem->setWaveSystem(waveSystem);
            
            // Create the game world
            createPlayer();
            
            // Set player entity for systems that need it
            enemySpawnSystem->setPlayerEntity(playerId);
            enemyAISystem->setPlayerEntity(playerId);
            shootingSystem->setPlayerEntity(playerId);
            collisionSystem->setPlayerEntity(playerId);
            shopSystem->setPlayerEntity(playerId);
            pickupSystem->setPlayerEntity(playerId);
            uiSystem->setPlayerEntity(playerId);
            
            setupEventSubscriptions();
            
            std::cout << "GameplayManager initialized successfully!" << std::endl;
            std::cout << "Player created with ID: " << playerId << std::endl;
            
            // Start the first wave immediately (no shop before Wave 1)
            if (waveSystem) {
                waveSystem->startWave();
            }
            
            return true;
        }
        
        void GameplayManager::update(float deltaTime) {
            gameTime += deltaTime;
            
            // Check if player is dead - stop updating if so (like original game)
            if (!isPlayerAlive()) {
                return; // Stop all game updates when player dies (main loop will handle exit)
            }
            
            // Update wave system first (manages game state)
            if (waveSystem) {
                waveSystem->update(deltaTime);
                
                // Check if we need to open shop after wave completion
                if (waveSystem->getWaveState() == WaveState::COMPLETED && shopSystem && !shopSystem->isShopActive()) {
                    shopSystem->openShop(waveSystem->getCurrentWave() - 1); // Shop opens for the wave just completed
                }
            }
            
            // Update shop system
            if (shopSystem) {
                shopSystem->update(deltaTime);
            }
            
            // Only update gameplay systems when wave is active (not during shop time)
            bool waveActive = waveSystem && waveSystem->isWaveActive();
            
            if (movementSystem) {
                movementSystem->update(deltaTime);
            }
            
            if (waveActive && enemySpawnSystem) {
                enemySpawnSystem->update(deltaTime);
            }
            
            if (waveActive && enemyAISystem) {
                enemyAISystem->update(deltaTime);
            }
            
            if (waveActive && shootingSystem) {
                shootingSystem->update(deltaTime);
            }
            
            if (waveActive && projectileSystem) {
                projectileSystem->update(deltaTime);
            }
            
            if (waveActive && collisionSystem) {
                collisionSystem->update(deltaTime);
            }
            
            // Always update pickup system (works during waves and shop)
            if (pickupSystem) {
                pickupSystem->update(deltaTime);
            }
        }
        
        void GameplayManager::render() {
            // Render all gameplay elements
            if (renderSystem) {
                renderSystem->render();
            }
            
            // Render UI on top of gameplay
            if (uiSystem) {
                uiSystem->render();
            }
            
            // Render shop on top if active
            if (shopSystem && shopSystem->isShopActive()) {
                shopSystem->render();
            }
        }
        
        void GameplayManager::shutdown() {
            if (renderSystem) renderSystem->shutdown();
            if (movementSystem) movementSystem->shutdown();
            if (enemySpawnSystem) enemySpawnSystem->shutdown();
            if (enemyAISystem) enemyAISystem->shutdown();
            if (projectileSystem) projectileSystem->shutdown();
            if (shootingSystem) shootingSystem->shutdown();
            if (collisionSystem) collisionSystem->shutdown();
            if (waveSystem) waveSystem->shutdown();
            if (shopSystem) shopSystem->shutdown();
            if (pickupSystem) pickupSystem->shutdown();
            if (uiSystem) uiSystem->shutdown();
            
            std::cout << "GameplayManager shutdown." << std::endl;
        }
        
        void GameplayManager::handleShopKeyboard(const Uint8* keyState) {
            if (shopSystem && shopSystem->isShopActive()) {
                shopSystem->handleKeyboardState(keyState);
            }
        }
        
        bool GameplayManager::isPlayerAlive() const {
            if (playerId == Core::INVALID_ENTITY_ID) {
                return false; // No player entity
            }
            
            auto playerHealth = entityManager->getComponent<Health>(playerId);
            return playerHealth && playerHealth->isAlive();
        }
        
        void GameplayManager::createPlayer() {
            // Create player entity
            playerId = entityManager->createEntity();
            
            // Add Transform component (spawn in center of screen)
            Vector2 playerStartPos(1920.0f / 2.0f, 1080.0f / 2.0f);
            entityManager->addComponent<Transform>(playerId, Transform(playerStartPos));
            
            // Add Movement component
            Movement playerMovement(200.0f); // 200 speed like original
            entityManager->addComponent<Movement>(playerId, playerMovement);
            
            // Add Player component
            Player playerComponent;
            playerComponent.level = 1;
            playerComponent.experience = 0;
            playerComponent.materials = 50;
            playerComponent.pickupRange = 50.0f;
            playerComponent.aimDirection = Vector2(1, 0);
            entityManager->addComponent<Player>(playerId, playerComponent);
            
            // Add Health component
            Health playerHealth(100);
            entityManager->addComponent<Health>(playerId, playerHealth);
            
            std::cout << "Player created at position: " << playerStartPos.x << ", " << playerStartPos.y << std::endl;
        }
        
        void GameplayManager::setupEventSubscriptions() {
            auto eventManager = Core::Services::getInstance().getService<Core::EventManager>();
            if (!eventManager) {
                std::cout << "GameplayManager: Failed to get event manager service!" << std::endl;
                return;
            }
            
            // Subscribe to input events for gameplay actions
            eventManager->subscribe<Core::InputActionEvent>([this](const Core::InputActionEvent& event) {
                if (event.action == Core::InputAction::FIRE && event.pressed) {
                    // TODO: Create projectile
                    std::cout << "Fire button pressed!" << std::endl;
                }
            });
            
            std::cout << "GameplayManager: Event subscriptions set up." << std::endl;
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
