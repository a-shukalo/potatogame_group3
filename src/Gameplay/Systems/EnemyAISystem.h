#pragma once
#include "../../Core/Engine/Application.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        class EnemyAISystem : public Core::ISystem {
        public:
            EnemyAISystem();
            ~EnemyAISystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            void setPlayerEntity(EntityId player) { playerEntity = player; }
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            EntityId playerEntity;
            
            void updateEnemyAI(EntityId enemyId, Transform& enemyTransform, Movement& enemyMovement, Enemy& enemy, float deltaTime);
            Vector2 calculateDirectionToPlayer(const Vector2& enemyPos, const Vector2& playerPos);
            void applyScreenBounds(Transform& transform, float radius);
            
            // Screen boundaries
            static const int WINDOW_WIDTH = 1920;
            static const int WINDOW_HEIGHT = 1080;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
