#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Core/Events/EventManager.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        // Forward declaration
        class PickupSystem;
        class WaveSystem;
        
        class CollisionSystem : public Core::ISystem {
        public:
            CollisionSystem();
            ~CollisionSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            void setPlayerEntity(EntityId player) { playerEntity = player; }
            void setPickupSystem(std::shared_ptr<PickupSystem> ps) { pickupSystem = ps; }
            void setWaveSystem(std::shared_ptr<WaveSystem> ws) { waveSystem = ws; }
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<PickupSystem> pickupSystem;
            std::shared_ptr<WaveSystem> waveSystem;
            EntityId playerEntity;
            
            // Collision detection methods
            void checkBulletEnemyCollisions();
            void checkEnemyPlayerCollisions();
            
            // Utility methods
            bool isCircleCollision(const Vector2& pos1, float radius1, const Vector2& pos2, float radius2) const;
            float getDistance(const Vector2& pos1, const Vector2& pos2) const;
            
            // Damage and destruction
            void destroyEnemy(EntityId enemyId);
            void damagePlayer(EntityId playerId, int damage);
            
            // Default radii
            static const float PLAYER_RADIUS;
            static const float ENEMY_RADIUS;
            static const float BULLET_RADIUS;
            
            // Damage settings
            static const int ENEMY_CONTACT_DAMAGE;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
