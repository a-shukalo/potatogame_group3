#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Core/Events/EventManager.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"
#include "ProjectileSystem.h"
#include <memory>

namespace BrotatoGame {
    namespace Gameplay {
        
        class ShootingSystem : public Core::ISystem {
        public:
            ShootingSystem();
            ~ShootingSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            void setProjectileSystem(std::shared_ptr<ProjectileSystem> ps) { projectileSystem = ps; }
            void setPlayerEntity(EntityId player) { playerEntity = player; }
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<ProjectileSystem> projectileSystem;
            EntityId playerEntity;
            
            void updatePlayerWeapons(EntityId playerId, Transform& playerTransform, Player& player, float deltaTime);
            void fireWeapon(const Vector2& playerPos, const Vector2& aimDirection, const Weapon& weapon, const Player& player);
            void performMeleeAttack(const Vector2& playerPos, const Vector2& aimDirection, const Weapon& weapon, const Player& player);
            bool canWeaponFire(const Weapon& weapon, float currentTime) const;
            int calculateWeaponDamage(const Weapon& weapon, const Player& player) const;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
