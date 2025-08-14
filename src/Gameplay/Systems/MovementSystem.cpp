#include "MovementSystem.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>

namespace BrotatoGame {
    namespace Gameplay {
        
        MovementSystem::MovementSystem() {}
        
        bool MovementSystem::initialize() {
            inputManager = Core::Services::getInstance().getService<Input::InputManager>();
            if (!inputManager) {
                std::cout << "MovementSystem: Failed to get input manager service!" << std::endl;
                return false;
            }
            
            std::cout << "MovementSystem initialized successfully!" << std::endl;
            return true;
        }
        
        void MovementSystem::update(float deltaTime) {
            if (!entityManager || !inputManager) return;
            
            // Update player movement
            auto playerEntities = entityManager->getEntitiesWith<Transform, Movement, Player>();
            for (EntityId entity : playerEntities) {
                auto transform = entityManager->getComponent<Transform>(entity);
                auto movement = entityManager->getComponent<Movement>(entity);
                auto player = entityManager->getComponent<Player>(entity);
                
                if (transform && movement && player) {
                    updatePlayerMovement(entity, *transform, *movement, *player, deltaTime);
                }
            }
            
            // Update projectile movement
            auto projectileEntities = entityManager->getEntitiesWith<Transform, Projectile>();
            for (EntityId entity : projectileEntities) {
                auto transform = entityManager->getComponent<Transform>(entity);
                auto projectile = entityManager->getComponent<Projectile>(entity);
                
                if (transform && projectile) {
                    updateProjectileMovement(entity, *transform, *projectile, deltaTime);
                }
            }
        }
        
        void MovementSystem::shutdown() {
            std::cout << "MovementSystem shutdown." << std::endl;
        }
        
        void MovementSystem::updatePlayerMovement(EntityId entity, Transform& transform, Movement& movement, Player& player, float deltaTime) {
            Vector2 inputDirection(0, 0);
            
            // Get input from InputManager
            if (inputManager->isActionActive(Core::InputAction::MOVE_UP)) {
                inputDirection.y -= 1.0f;
            }
            if (inputManager->isActionActive(Core::InputAction::MOVE_DOWN)) {
                inputDirection.y += 1.0f;
            }
            if (inputManager->isActionActive(Core::InputAction::MOVE_LEFT)) {
                inputDirection.x -= 1.0f;
            }
            if (inputManager->isActionActive(Core::InputAction::MOVE_RIGHT)) {
                inputDirection.x += 1.0f;
            }
            
            // Normalize diagonal movement
            if (inputDirection.length() > 0.01f) {
                inputDirection.normalize();
            }
            
            // Apply movement
            Vector2 targetVelocity = inputDirection * movement.speed;
            
            // Simple movement (no acceleration for now)
            movement.velocity = targetVelocity;
            transform.position += movement.velocity * deltaTime;
            
            // Apply screen bounds
            applyScreenBounds(transform, 20.0f); // Player radius
            
            // Update aim direction to mouse position
            Vector2 mousePos = inputManager->getMousePosition();
            Vector2 aimDir = mousePos - transform.position;
            if (aimDir.length() > 0.1f) {
                player.aimDirection = aimDir.normalized();
            }
        }
        
        void MovementSystem::updateProjectileMovement(EntityId entity, Transform& transform, Projectile& projectile, float deltaTime) {
            // Move projectile
            Vector2 movement = projectile.direction * projectile.speed * deltaTime;
            transform.position += movement;
            projectile.travelDistance += movement.length();
            
            // Check if projectile is expired
            if (projectile.isExpired()) {
                entityManager->destroyEntity(entity);
            }
        }
        
        void MovementSystem::applyScreenBounds(Transform& transform, float radius) {
            if (transform.position.x < radius) transform.position.x = radius;
            if (transform.position.x > WINDOW_WIDTH - radius) transform.position.x = WINDOW_WIDTH - radius;
            if (transform.position.y < radius) transform.position.y = radius;
            if (transform.position.y > WINDOW_HEIGHT - radius) transform.position.y = WINDOW_HEIGHT - radius;
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
