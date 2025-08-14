#include "PickupSystem.h"
#include <iostream>
#include <cmath>

namespace BrotatoGame {
    namespace Gameplay {
        
        PickupSystem::PickupSystem() 
            : playerEntity(Core::INVALID_ENTITY_ID)
        {
        }
        
        bool PickupSystem::initialize() {
            std::cout << "PickupSystem initialized successfully!" << std::endl;
            return true;
        }
        
        void PickupSystem::update(float deltaTime) {
            if (!entityManager || playerEntity == Core::INVALID_ENTITY_ID) {
                return;
            }
            
            // Update all pickup-related entities
            updateExperienceOrbs(deltaTime);
            updateMaterials(deltaTime);
            updatePickupCollection();
            updatePickupLifetimes(deltaTime);
        }
        
        void PickupSystem::shutdown() {
            std::cout << "PickupSystem shutdown." << std::endl;
        }
        
        EntityId PickupSystem::createExperienceOrb(const Vector2& position, int experienceValue) {
            EntityId orbId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(orbId, Transform(position));
            
            // Add ExperienceOrb component
            entityManager->addComponent<ExperienceOrb>(orbId, ExperienceOrb(experienceValue));
            
            std::cout << "Created experience orb with " << experienceValue << " XP at (" << position.x << ", " << position.y << ")" << std::endl;
            
            return orbId;
        }
        
        EntityId PickupSystem::createMaterial(const Vector2& position, int materialValue, int experienceValue) {
            EntityId materialId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(materialId, Transform(position));
            
            // Add Material component
            entityManager->addComponent<Material>(materialId, Material(materialValue, experienceValue));
            
            std::cout << "Created material with " << materialValue << " mats + " << experienceValue << " XP at (" << position.x << ", " << position.y << ")" << std::endl;
            
            return materialId;
        }
        
        void PickupSystem::updateExperienceOrbs(float deltaTime) {
            auto orbs = entityManager->getEntitiesWith<Transform, ExperienceOrb>();
            
            for (EntityId orbId : orbs) {
                auto orbComponent = entityManager->getComponent<ExperienceOrb>(orbId);
                if (orbComponent && !orbComponent->collected) {
                    orbComponent->lifetime -= deltaTime;
                    
                    // Remove expired orbs
                    if (orbComponent->isExpired()) {
                        entityManager->destroyEntity(orbId);
                    }
                }
            }
        }
        
        void PickupSystem::updateMaterials(float deltaTime) {
            auto materials = entityManager->getEntitiesWith<Transform, Material>();
            
            for (EntityId materialId : materials) {
                auto materialComponent = entityManager->getComponent<Material>(materialId);
                if (materialComponent && !materialComponent->collected) {
                    materialComponent->lifetime -= deltaTime;
                    
                    // Remove expired materials
                    if (materialComponent->isExpired()) {
                        entityManager->destroyEntity(materialId);
                    }
                }
            }
        }
        
        void PickupSystem::updatePickupCollection() {
            auto playerTransform = entityManager->getComponent<Transform>(playerEntity);
            auto player = entityManager->getComponent<Player>(playerEntity);
            
            if (!playerTransform || !player) return;
            
            Vector2 playerPos = playerTransform->position;
            float pickupRange = player->pickupRange;
            
            // Collect experience orbs
            auto orbs = entityManager->getEntitiesWith<Transform, ExperienceOrb>();
            for (EntityId orbId : orbs) {
                auto orbTransform = entityManager->getComponent<Transform>(orbId);
                auto orb = entityManager->getComponent<ExperienceOrb>(orbId);
                
                if (orbTransform && orb && !orb->collected) {
                    if (isWithinPickupRange(playerPos, orbTransform->position, pickupRange)) {
                        collectExperienceOrb(orbId, *orb);
                    }
                }
            }
            
            // Collect materials
            auto materials = entityManager->getEntitiesWith<Transform, Material>();
            for (EntityId materialId : materials) {
                auto materialTransform = entityManager->getComponent<Transform>(materialId);
                auto material = entityManager->getComponent<Material>(materialId);
                
                if (materialTransform && material && !material->collected) {
                    if (isWithinPickupRange(playerPos, materialTransform->position, pickupRange)) {
                        collectMaterial(materialId, *material);
                    }
                }
            }
        }
        
        void PickupSystem::updatePickupLifetimes(float deltaTime) {
            // This is handled in updateExperienceOrbs and updateMaterials
        }
        
        void PickupSystem::collectExperienceOrb(EntityId orbId, const ExperienceOrb& orb) {
            auto player = entityManager->getComponent<Player>(playerEntity);
            auto health = entityManager->getComponent<Health>(playerEntity);
            
            if (player) {
                // Gain experience
                player->gainExperience(orb.experienceValue);
                
                // Check for level up
                if (player->canLevelUp()) {
                    player->levelUp();
                    applyPlayerLevelUp(*player, *health);
                }
            }
            
            // Remove the orb
            entityManager->destroyEntity(orbId);
        }
        
        void PickupSystem::collectMaterial(EntityId materialId, const Material& material) {
            auto player = entityManager->getComponent<Player>(playerEntity);
            auto health = entityManager->getComponent<Health>(playerEntity);
            
            if (player) {
                // Gain materials and experience
                player->gainMaterials(material.materialValue);
                player->gainExperience(material.experienceValue);
                
                // Check for level up
                if (player->canLevelUp()) {
                    player->levelUp();
                    applyPlayerLevelUp(*player, *health);
                }
            }
            
            // Remove the material
            entityManager->destroyEntity(materialId);
        }
        
        void PickupSystem::applyPlayerLevelUp(Player& player, Health& health) {
            // Full heal on level up (like original game)
            health.current = player.maxHealth;
            health.maximum = player.maxHealth; // Update max health too
            
            std::cout << "Player fully healed! HP: " << health.current << "/" << health.maximum << std::endl;
            
            // TODO: Apply other stat bonuses to other systems
            // For now, the player component handles the stat increases directly
        }
        
        float PickupSystem::getDistance(const Vector2& pos1, const Vector2& pos2) const {
            Vector2 diff = pos2 - pos1;
            return std::sqrt(diff.x * diff.x + diff.y * diff.y);
        }
        
        bool PickupSystem::isWithinPickupRange(const Vector2& playerPos, const Vector2& itemPos, float pickupRange) const {
            return getDistance(playerPos, itemPos) <= pickupRange;
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
