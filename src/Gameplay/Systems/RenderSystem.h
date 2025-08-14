#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Rendering/Renderer/IRenderer.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        class RenderSystem : public Core::ISystem {
        public:
            RenderSystem();
            ~RenderSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void render() override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<Rendering::IRenderer> renderer;
            
            // Texture cache
            Rendering::TextureHandle playerTexture = nullptr;
            Rendering::TextureHandle enemySlimeTexture = nullptr;
            Rendering::TextureHandle enemyPebblinTexture = nullptr;
            Rendering::TextureHandle bulletTexture = nullptr;
            
            void loadTextures();
            
            void renderEntity(EntityId entity);
            void renderPlayer(EntityId entity, const Transform& transform, const Player& player);
            void renderEnemy(EntityId entity, const Transform& transform, const Enemy& enemy);
            void renderProjectile(EntityId entity, const Transform& transform, const Projectile& projectile);
            void renderExperienceOrb(EntityId entity, const Transform& transform, const ExperienceOrb& orb);
            void renderMaterial(EntityId entity, const Transform& transform, const Material& material);
            void renderSpawnIndicator(EntityId entity, const Transform& transform, const SpawnIndicator& indicator);
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
