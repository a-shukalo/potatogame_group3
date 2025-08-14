#pragma once
#include "../../Core/Engine/Application.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        class PickupSystem : public Core::ISystem {
        public:
            PickupSystem();
            ~PickupSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            void setPlayerEntity(EntityId player) { playerEntity = player; }
            
            // Create pickup entities
            EntityId createExperienceOrb(const Vector2& position, int experienceValue = 1);
            EntityId createMaterial(const Vector2& position, int materialValue = 2, int experienceValue = 1);
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            EntityId playerEntity;
            
            // Update systems
            void updateExperienceOrbs(float deltaTime);
            void updateMaterials(float deltaTime);
            void updatePickupCollection();
            void updatePickupLifetimes(float deltaTime);
            
            // Collection logic
            void collectExperienceOrb(EntityId orbId, const ExperienceOrb& orb);
            void collectMaterial(EntityId materialId, const Material& material);
            void applyPlayerLevelUp(Player& player, Health& health);
            
            // Utility
            float getDistance(const Vector2& pos1, const Vector2& pos2) const;
            bool isWithinPickupRange(const Vector2& playerPos, const Vector2& itemPos, float pickupRange) const;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
