#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Core/Events/EventManager.h"
#include "../../Input/InputSystem/InputManager.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        class MovementSystem : public Core::ISystem {
        public:
            MovementSystem();
            ~MovementSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<Input::InputManager> inputManager;
            
            void updatePlayerMovement(EntityId entity, Transform& transform, Movement& movement, Player& player, float deltaTime);
            void updateProjectileMovement(EntityId entity, Transform& transform, Projectile& projectile, float deltaTime);
            void applyScreenBounds(Transform& transform, float radius);
            
            // Screen boundaries
            static const int WINDOW_WIDTH = 1920;
            static const int WINDOW_HEIGHT = 1080;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
