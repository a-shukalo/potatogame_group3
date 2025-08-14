#include "EnemyAISystem.h"
#include <iostream>
#include <cmath>

namespace BrotatoGame {
    namespace Gameplay {
        
        EnemyAISystem::EnemyAISystem() 
            : playerEntity(Core::INVALID_ENTITY_ID)
        {
        }
        
        bool EnemyAISystem::initialize() {
            std::cout << "EnemyAISystem initialized successfully!" << std::endl;
            return true;
        }
        
        void EnemyAISystem::update(float deltaTime) {
            if (!entityManager || playerEntity == Core::INVALID_ENTITY_ID) {
                return;
            }
            
            // Get player position
            auto playerTransform = entityManager->getComponent<Transform>(playerEntity);
            if (!playerTransform) {
                return;
            }
            
            // Update all enemies
            auto enemies = entityManager->getEntitiesWith<Transform, Movement, Enemy>();
            for (EntityId enemyId : enemies) {
                auto enemyTransform = entityManager->getComponent<Transform>(enemyId);
                auto enemyMovement = entityManager->getComponent<Movement>(enemyId);
                auto enemy = entityManager->getComponent<Enemy>(enemyId);
                
                if (enemyTransform && enemyMovement && enemy) {
                    updateEnemyAI(enemyId, *enemyTransform, *enemyMovement, *enemy, deltaTime);
                }
            }
        }
        
        void EnemyAISystem::shutdown() {
            std::cout << "EnemyAISystem shutdown." << std::endl;
        }
        
        void EnemyAISystem::updateEnemyAI(EntityId enemyId, Transform& enemyTransform, Movement& enemyMovement, Enemy& enemy, float deltaTime) {
            // Get player position
            auto playerTransform = entityManager->getComponent<Transform>(playerEntity);
            if (!playerTransform) {
                return;
            }
            
            // Calculate direction to player
            Vector2 direction = calculateDirectionToPlayer(enemyTransform.position, playerTransform->position);
            
            // Set movement velocity toward player
            enemyMovement.velocity = direction * enemy.speed;
            
            // Apply movement
            enemyTransform.position += enemyMovement.velocity * deltaTime;
            
            // Keep enemies on screen
            applyScreenBounds(enemyTransform, 15.0f); // enemy radius
        }
        
        Vector2 EnemyAISystem::calculateDirectionToPlayer(const Vector2& enemyPos, const Vector2& playerPos) {
            Vector2 direction = playerPos - enemyPos;
            
            // Normalize the direction
            float length = direction.length();
            if (length > 0.1f) {
                direction = direction / length;
            } else {
                direction = Vector2(0, 0);
            }
            
            return direction;
        }
        
        void EnemyAISystem::applyScreenBounds(Transform& transform, float radius) {
            if (transform.position.x < radius) transform.position.x = radius;
            if (transform.position.x > WINDOW_WIDTH - radius) transform.position.x = WINDOW_WIDTH - radius;
            if (transform.position.y < radius) transform.position.y = radius;
            if (transform.position.y > WINDOW_HEIGHT - radius) transform.position.y = WINDOW_HEIGHT - radius;
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
