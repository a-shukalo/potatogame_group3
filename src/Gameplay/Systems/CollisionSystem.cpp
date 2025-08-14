#include "CollisionSystem.h"
#include "PickupSystem.h"
#include "WaveSystem.h"
#include <iostream>
#include <cmath>
#include <random>

namespace BrotatoGame {
    namespace Gameplay {
        
        // Static constants
        const float CollisionSystem::PLAYER_RADIUS = 20.0f;
        const float CollisionSystem::ENEMY_RADIUS = 15.0f;
        const float CollisionSystem::BULLET_RADIUS = 3.0f;
        const int CollisionSystem::ENEMY_CONTACT_DAMAGE = 10;
        
        CollisionSystem::CollisionSystem() 
            : playerEntity(Core::INVALID_ENTITY_ID)
        {
        }
        
        bool CollisionSystem::initialize() {
            std::cout << "CollisionSystem initialized successfully!" << std::endl;
            return true;
        }
        
        void CollisionSystem::update(float deltaTime) {
            if (!entityManager || playerEntity == Core::INVALID_ENTITY_ID) {
                return;
            }
            
            // Check all collision types
            checkBulletEnemyCollisions();
            checkEnemyPlayerCollisions();
        }
        
        void CollisionSystem::shutdown() {
            std::cout << "CollisionSystem shutdown." << std::endl;
        }
        
        void CollisionSystem::checkBulletEnemyCollisions() {
            // Get all bullets and enemies
            auto bullets = entityManager->getEntitiesWith<Transform, Projectile>();
            auto enemies = entityManager->getEntitiesWith<Transform, Enemy>();
            
            for (EntityId bulletId : bullets) {
                auto bulletTransform = entityManager->getComponent<Transform>(bulletId);
                auto projectile = entityManager->getComponent<Projectile>(bulletId);
                
                if (!bulletTransform || !projectile) continue;
                
                for (EntityId enemyId : enemies) {
                    auto enemyTransform = entityManager->getComponent<Transform>(enemyId);
                    auto enemy = entityManager->getComponent<Enemy>(enemyId);
                    
                    if (!enemyTransform || !enemy) continue;
                    
                    // Check collision between bullet and enemy
                    if (isCircleCollision(bulletTransform->position, BULLET_RADIUS, 
                                        enemyTransform->position, ENEMY_RADIUS)) {
                        
                        std::cout << "Bullet hit enemy! Damage: " << projectile->damage << std::endl;
                        
                        // Apply damage to enemy
                        auto enemyHealth = entityManager->getComponent<Health>(enemyId);
                        if (enemyHealth) {
                            enemyHealth->takeDamage(projectile->damage);
                            
                            // Check if enemy is dead
                            if (!enemyHealth->isAlive()) {
                                std::cout << "Enemy destroyed!" << std::endl;
                                destroyEnemy(enemyId);
                            }
                        }
                        
                        // Destroy bullet
                        entityManager->destroyEntity(bulletId);
                        break; // Bullet is destroyed, no need to check more enemies
                    }
                }
            }
        }
        
        void CollisionSystem::checkEnemyPlayerCollisions() {
            auto playerTransform = entityManager->getComponent<Transform>(playerEntity);
            auto playerHealth = entityManager->getComponent<Health>(playerEntity);
            
            // Don't process collisions if player is dead or missing components
            if (!playerTransform || !playerHealth || !playerHealth->isAlive()) {
                return;
            }
            
            auto enemies = entityManager->getEntitiesWith<Transform, Enemy>();
            
            for (EntityId enemyId : enemies) {
                auto enemyTransform = entityManager->getComponent<Transform>(enemyId);
                if (!enemyTransform) continue;
                
                // Check collision between enemy and player
                if (isCircleCollision(enemyTransform->position, ENEMY_RADIUS,
                                    playerTransform->position, PLAYER_RADIUS)) {
                    
                    std::cout << "Enemy hit player! Damage: " << ENEMY_CONTACT_DAMAGE << std::endl;
                    
                    // Apply damage to player
                    damagePlayer(playerEntity, ENEMY_CONTACT_DAMAGE);
                    
                    // Push enemy away slightly to prevent continuous damage
                    Vector2 pushDirection = (enemyTransform->position - playerTransform->position).normalized();
                    enemyTransform->position += pushDirection * 5.0f; // Push 5 pixels away
                    
                    // Stop processing if player died from this hit
                    if (!playerHealth->isAlive()) {
                        std::cout << "Player died - stopping collision processing" << std::endl;
                        return;
                    }
                }
            }
        }
        
        bool CollisionSystem::isCircleCollision(const Vector2& pos1, float radius1, const Vector2& pos2, float radius2) const {
            float distance = getDistance(pos1, pos2);
            return distance <= (radius1 + radius2);
        }
        
        float CollisionSystem::getDistance(const Vector2& pos1, const Vector2& pos2) const {
            Vector2 diff = pos2 - pos1;
            return std::sqrt(diff.x * diff.x + diff.y * diff.y);
        }
        
        void CollisionSystem::destroyEnemy(EntityId enemyId) {
            // Get enemy position for drop placement
            auto enemyTransform = entityManager->getComponent<Transform>(enemyId);
            auto enemy = entityManager->getComponent<Enemy>(enemyId);
            
            if (enemyTransform && enemy && pickupSystem) {
                Vector2 dropPosition = enemyTransform->position;
                
                // Always drop experience (like original game)
                int xpValue = 1; // Base XP
                if (waveSystem) {
                    // Scale XP with wave number (like original: +1 XP per 5 waves)
                    int currentWave = waveSystem->getCurrentWave();
                    xpValue = 1 + (currentWave / 5);
                }
                pickupSystem->createExperienceOrb(dropPosition, xpValue);
                
                // Drop materials based on original game's drop chance
                float materialDropChance = 0.7f; // 70% base chance
                if (waveSystem) {
                    // Higher waves have better drop rates
                    int currentWave = waveSystem->getCurrentWave();
                    materialDropChance += (currentWave * 0.05f); // +5% per wave
                    if (materialDropChance > 0.95f) materialDropChance = 0.95f; // Cap at 95%
                }
                
                // Random material drop
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                
                if (dist(gen) < materialDropChance) {
                    int materialValue = 2; // Base materials
                    if (waveSystem) {
                        // Scale materials with wave: 2 + (wave / 3)
                        int currentWave = waveSystem->getCurrentWave();
                        materialValue = 2 + (currentWave / 3);
                    }
                    
                    pickupSystem->createMaterial(dropPosition, materialValue, 1);
                }
                
                std::cout << "Enemy defeated! Dropped " << xpValue << " XP";
                if (dist(gen) < materialDropChance) {
                    std::cout << " + materials";
                }
                std::cout << std::endl;
            }
            
            // Destroy the enemy
            entityManager->destroyEntity(enemyId);
        }
        
        void CollisionSystem::damagePlayer(EntityId playerId, int damage) {
            auto playerHealth = entityManager->getComponent<Health>(playerId);
            if (playerHealth) {
                playerHealth->takeDamage(damage);
                
                std::cout << "Player health: " << playerHealth->current << "/" << playerHealth->maximum << std::endl;
                
                if (!playerHealth->isAlive()) {
                    std::cout << "GAME OVER! Player died!" << std::endl;
                    // TODO: Handle game over
                }
            }
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
