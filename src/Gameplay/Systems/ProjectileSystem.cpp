#include "ProjectileSystem.h"
#include <iostream>

namespace BrotatoGame {
    namespace Gameplay {
        
        ProjectileSystem::ProjectileSystem() {}
        
        bool ProjectileSystem::initialize() {
            std::cout << "ProjectileSystem initialized successfully!" << std::endl;
            return true;
        }
        
        void ProjectileSystem::update(float deltaTime) {
            if (!entityManager) return;
            
            // Update all projectiles
            auto projectiles = entityManager->getEntitiesWith<Transform, Projectile>();
            for (EntityId bulletId : projectiles) {
                auto transform = entityManager->getComponent<Transform>(bulletId);
                auto projectile = entityManager->getComponent<Projectile>(bulletId);
                
                if (transform && projectile) {
                    updateProjectileMovement(bulletId, *transform, *projectile, deltaTime);
                    
                    // Check if projectile is expired
                    if (projectile->isExpired()) {
                        entityManager->destroyEntity(bulletId);
                    }
                }
            }
        }
        
        void ProjectileSystem::shutdown() {
            std::cout << "ProjectileSystem shutdown." << std::endl;
        }
        
        EntityId ProjectileSystem::createBullet(const Vector2& position, const Vector2& direction, int damage, float range, float speed) {
            EntityId bulletId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(bulletId, Transform(position));
            
            // Add Projectile component
            Projectile projectileComponent;
            projectileComponent.direction = direction.normalized();
            projectileComponent.speed = speed;
            projectileComponent.damage = damage;
            projectileComponent.maxRange = range;
            projectileComponent.travelDistance = 0.0f;
            projectileComponent.lifetime = range / speed; // Calculate lifetime based on range and speed
            entityManager->addComponent<Projectile>(bulletId, projectileComponent);
            
            std::cout << "Created bullet with damage: " << damage << " at position: " << position.x << ", " << position.y << std::endl;
            
            return bulletId;
        }
        
        void ProjectileSystem::updateProjectileMovement(EntityId bulletId, Transform& transform, Projectile& projectile, float deltaTime) {
            // Move projectile
            Vector2 movement = projectile.direction * projectile.speed * deltaTime;
            transform.position += movement;
            projectile.travelDistance += movement.length();
            projectile.lifetime -= deltaTime;
            
            // Keep bullets slightly on screen (they can go a bit off-screen before cleanup)
            applyScreenBounds(transform, 5.0f);
        }
        
        void ProjectileSystem::applyScreenBounds(Transform& transform, float radius) {
            // Allow bullets to go slightly off-screen before being cleaned up
            if (transform.position.x < -50.0f || transform.position.x > WINDOW_WIDTH + 50.0f ||
                transform.position.y < -50.0f || transform.position.y > WINDOW_HEIGHT + 50.0f) {
                // Don't constrain, let the lifetime system handle cleanup
            }
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
