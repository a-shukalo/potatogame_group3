#pragma once
#include "../../Core/Engine/Application.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        class ProjectileSystem : public Core::ISystem {
        public:
            ProjectileSystem();
            ~ProjectileSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            
            // Create bullet from weapon
            EntityId createBullet(const Vector2& position, const Vector2& direction, int damage, float range, float speed);
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            
            void updateProjectileMovement(EntityId bulletId, Transform& transform, Projectile& projectile, float deltaTime);
            void applyScreenBounds(Transform& transform, float radius);
            
            // Screen boundaries
            static const int WINDOW_WIDTH = 1920;
            static const int WINDOW_HEIGHT = 1080;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
