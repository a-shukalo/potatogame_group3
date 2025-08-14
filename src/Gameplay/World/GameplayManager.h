#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Core/Events/EventManager.h"
#include "../Entities/EntityManager.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/EnemySpawnSystem.h"
#include "../Systems/EnemyAISystem.h"
#include "../Systems/ProjectileSystem.h"
#include "../Systems/ShootingSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/WaveSystem.h"
#include "../Systems/ShopSystem.h"
#include "../Systems/PickupSystem.h"
#include "../Systems/UISystem.h"
#include <memory>
#include <SDL2/SDL.h>

namespace BrotatoGame {
    namespace Gameplay {
        
        class GameplayManager : public Core::ISystem {
        public:
            GameplayManager();
            ~GameplayManager() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void render() override;
            void shutdown() override;
        
            // Game state queries
            bool isPlayerAlive() const;
        
        // Input handling for shop
        void handleShopKeyboard(const Uint8* keyState);
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<RenderSystem> renderSystem;
            std::shared_ptr<MovementSystem> movementSystem;
            std::shared_ptr<EnemySpawnSystem> enemySpawnSystem;
            std::shared_ptr<EnemyAISystem> enemyAISystem;
            std::shared_ptr<ProjectileSystem> projectileSystem;
            std::shared_ptr<ShootingSystem> shootingSystem;
            std::shared_ptr<CollisionSystem> collisionSystem;
            std::shared_ptr<WaveSystem> waveSystem;
            std::shared_ptr<ShopSystem> shopSystem;
            std::shared_ptr<PickupSystem> pickupSystem;
            std::shared_ptr<UISystem> uiSystem;
            
            // Game state
            EntityId playerId;
            float gameTime;
            
            // Initialization
            void createPlayer();
            void setupEventSubscriptions();
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
