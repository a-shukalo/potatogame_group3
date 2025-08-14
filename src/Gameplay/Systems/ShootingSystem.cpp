#include "ShootingSystem.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>
#include <cmath>
#include <random>

namespace BrotatoGame {
    namespace Gameplay {
        
        ShootingSystem::ShootingSystem() 
            : playerEntity(Core::INVALID_ENTITY_ID)
        {
        }
        
        bool ShootingSystem::initialize() {
            std::cout << "ShootingSystem initialized successfully!" << std::endl;
            return true;
        }
        
        void ShootingSystem::update(float deltaTime) {
            if (!entityManager || !projectileSystem || playerEntity == Core::INVALID_ENTITY_ID) {
                return;
            }
            
            // Update player weapons
            auto playerTransform = entityManager->getComponent<Transform>(playerEntity);
            auto player = entityManager->getComponent<Player>(playerEntity);
            
            if (playerTransform && player) {
                updatePlayerWeapons(playerEntity, *playerTransform, *player, deltaTime);
            }
        }
        
        void ShootingSystem::shutdown() {
            std::cout << "ShootingSystem shutdown." << std::endl;
        }
        
        void ShootingSystem::updatePlayerWeapons(EntityId playerId, Transform& playerTransform, Player& player, float deltaTime) {
            // Check if player has weapons (for now, give them a basic weapon if they don't have one)
            if (!entityManager->hasComponent<Weapon>(playerId)) {
                // Give player a basic weapon
                Weapon basicWeapon;
                basicWeapon.type = WeaponType::PISTOL;
                basicWeapon.damage = 25;
                basicWeapon.fireRate = 2.0f; // 2 shots per second
                basicWeapon.lastShotTime = 0.0f;
                entityManager->addComponent<Weapon>(playerId, basicWeapon);
                std::cout << "Gave player a basic pistol!" << std::endl;
            }
            
            auto weapon = entityManager->getComponent<Weapon>(playerId);
            if (!weapon) return;
            
            // Update weapon cooldown
            weapon->lastShotTime += deltaTime;
            
            // Check if player is trying to fire (we'll use a simple auto-fire for now)
            // In the original, this was triggered by input events
            bool shouldFire = true; // Auto-fire for now
            
            if (shouldFire && canWeaponFire(*weapon, weapon->lastShotTime)) {
                fireWeapon(playerTransform.position, player.aimDirection, *weapon, player);
                weapon->lastShotTime = 0.0f; // Reset cooldown
            }
        }
        
        void ShootingSystem::fireWeapon(const Vector2& playerPos, const Vector2& aimDirection, const Weapon& weapon, const Player& player) {
            // Handle melee weapons differently
            if (weapon.type == WeaponType::MELEE_STICK) {
                performMeleeAttack(playerPos, aimDirection, weapon, player);
                return;
            }
            
            // Handle ranged weapons
            Vector2 bulletSpawnPos = playerPos + aimDirection * 25.0f;
            int damage = calculateWeaponDamage(weapon, player);
            
            // Handle different weapon types
            if (weapon.type == WeaponType::SHOTGUN) {
                // Shotgun fires multiple pellets
                int pellets = weapon.pelletsPerShot;
                float spreadAngle = weapon.spread;
                
                for (int i = 0; i < pellets; i++) {
                    float angle = atan2(aimDirection.y, aimDirection.x);
                    float pelletSpread = (i - pellets / 2.0f) * spreadAngle / pellets;
                    Vector2 pelletDirection(cos(angle + pelletSpread), sin(angle + pelletSpread));
                    
                    projectileSystem->createBullet(bulletSpawnPos, pelletDirection, damage, weapon.range, 400.0f);
                }
            } else {
                // Single shot weapons (pistol, SMG)
                float bulletSpeed = 400.0f;
                projectileSystem->createBullet(bulletSpawnPos, aimDirection, damage, weapon.range, bulletSpeed);
            }
        }
        
        void ShootingSystem::performMeleeAttack(const Vector2& playerPos, const Vector2& aimDirection, const Weapon& weapon, const Player& player) {
            // Calculate weapon tip position (extends from player position)
            Vector2 weaponTip = playerPos + aimDirection * weapon.range;
            
            // Calculate damage
            int damage = calculateWeaponDamage(weapon, player);
            
            // Check for critical hit
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<float> critRoll(0.0f, 1.0f);
            
            if (critRoll(gen) < weapon.critChance) {
                damage = static_cast<int>(damage * weapon.critMultiplier);
            }
            
            // Damage radius at weapon tip
            float damageRadius = 40.0f;
            
            // Find all enemies within damage radius and damage them
            auto enemies = entityManager->getEntitiesWith<Transform, Health, Enemy>();
            for (EntityId enemyId : enemies) {
                auto enemyTransform = entityManager->getComponent<Transform>(enemyId);
                auto enemyHealth = entityManager->getComponent<Health>(enemyId);
                
                if (!enemyTransform || !enemyHealth) continue;
                
                // Check distance to weapon tip
                float distance = (enemyTransform->position - weaponTip).length();
                if (distance <= damageRadius) {
                    // Apply damage
                    enemyHealth->current -= damage;
                    
                    // Apply knockback if enemy survives
                    if (enemyHealth->current > 0) {
                        Vector2 knockbackDirection = (enemyTransform->position - playerPos).normalized();
                        float knockbackForce = 150.0f; // Melee weapons have strong knockback
                        
                        // Apply knockback (simple velocity-based)
                        if (auto movement = entityManager->getComponent<Movement>(enemyId)) {
                            movement->velocity += knockbackDirection * knockbackForce;
                        }
                    }
                    
                    std::cout << "Melee attack hit enemy " << enemyId << " for " << damage << " damage! Health: " << enemyHealth->current << std::endl;
                }
            }
        }
        
        bool ShootingSystem::canWeaponFire(const Weapon& weapon, float currentTime) const {
            float cooldown = 1.0f / weapon.fireRate; // Convert fire rate to cooldown
            return currentTime >= cooldown;
        }
        
        int ShootingSystem::calculateWeaponDamage(const Weapon& weapon, const Player& player) const {
            // Basic damage calculation
            int baseDamage = weapon.damage;
            
            // Apply player damage modifiers if needed
            // For now, just return base damage
            return baseDamage;
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
