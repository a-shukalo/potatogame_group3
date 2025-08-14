#include "RenderSystem.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>
#include <cmath>

namespace BrotatoGame {
    namespace Gameplay {
        
        RenderSystem::RenderSystem() {}
        
        bool RenderSystem::initialize() {
            renderer = Core::Services::getInstance().getService<Rendering::IRenderer>();
            if (!renderer) {
                std::cout << "RenderSystem: Failed to get renderer service!" << std::endl;
                return false;
            }
            
            // Load textures from assets
            loadTextures();
            
            std::cout << "RenderSystem initialized successfully with renderer!" << std::endl;
            return true;
        }
        
        void RenderSystem::update(float deltaTime) {
            // RenderSystem doesn't need update logic, just rendering
        }
        
        void RenderSystem::render() {
            if (!entityManager || !renderer) {
                return;
            }
            
            // Render all entities that have Transform component
            auto entities = entityManager->getEntitiesWithComponent<Transform>();
            
            for (EntityId entity : entities) {
                renderEntity(entity);
            }
        }
        
        void RenderSystem::shutdown() {
            // Unload textures
            if (renderer) {
                if (playerTexture) renderer->unloadTexture(playerTexture);
                if (enemySlimeTexture) renderer->unloadTexture(enemySlimeTexture);
                if (enemyPebblinTexture) renderer->unloadTexture(enemyPebblinTexture);
                if (bulletTexture) renderer->unloadTexture(bulletTexture);
            }
            
            std::cout << "RenderSystem shutdown." << std::endl;
        }
        
        void RenderSystem::loadTextures() {
            if (!renderer) return;
            
            std::cout << "RenderSystem: Loading textures..." << std::endl;
            
            // Load player texture (brick character)
            playerTexture = renderer->loadTexture("assets/character/brick.png");
            if (playerTexture) {
                std::cout << "✓ Loaded player texture: brick.png" << std::endl;
            } else {
                std::cout << "✗ Failed to load player texture: brick.png" << std::endl;
            }
            
            // Load enemy textures
            enemySlimeTexture = renderer->loadTexture("assets/enemies/slime.png");
            if (enemySlimeTexture) {
                std::cout << "✓ Loaded slime texture" << std::endl;
            } else {
                std::cout << "✗ Failed to load slime texture" << std::endl;
            }
            
            enemyPebblinTexture = renderer->loadTexture("assets/enemies/pebblin.png");
            if (enemyPebblinTexture) {
                std::cout << "✓ Loaded pebblin texture" << std::endl;
            } else {
                std::cout << "✗ Failed to load pebblin texture" << std::endl;
            }
            
            // Load bullet texture
            bulletTexture = renderer->loadTexture("assets/weapons/small_bullet.png");
            if (bulletTexture) {
                std::cout << "✓ Loaded bullet texture" << std::endl;
            } else {
                std::cout << "✗ Failed to load bullet texture" << std::endl;
            }
            
            std::cout << "RenderSystem: Texture loading complete." << std::endl;
        }
        
        void RenderSystem::renderEntity(EntityId entity) {
            auto transform = entityManager->getComponent<Transform>(entity);
            if (!transform) return;
            
            // Render different entity types
            if (entityManager->hasComponent<Player>(entity)) {
                auto player = entityManager->getComponent<Player>(entity);
                renderPlayer(entity, *transform, *player);
            }
            else if (entityManager->hasComponent<Enemy>(entity)) {
                auto enemy = entityManager->getComponent<Enemy>(entity);
                renderEnemy(entity, *transform, *enemy);
            }
            else if (entityManager->hasComponent<Projectile>(entity)) {
                auto projectile = entityManager->getComponent<Projectile>(entity);
                renderProjectile(entity, *transform, *projectile);
            } else if (entityManager->hasComponent<ExperienceOrb>(entity)) {
                auto orb = entityManager->getComponent<ExperienceOrb>(entity);
                renderExperienceOrb(entity, *transform, *orb);
            } else if (entityManager->hasComponent<Material>(entity)) {
                auto material = entityManager->getComponent<Material>(entity);
                renderMaterial(entity, *transform, *material);
            } else if (entityManager->hasComponent<SpawnIndicator>(entity)) {
                auto indicator = entityManager->getComponent<SpawnIndicator>(entity);
                renderSpawnIndicator(entity, *transform, *indicator);
            }
        }
        
        void RenderSystem::renderPlayer(EntityId entity, const Transform& transform, const Player& player) {
            if (playerTexture) {
                // Render brick sprite with scaling
                float scale = 0.8f; // Same scale as original game
                Rendering::Transform renderTransform;
                renderTransform.position = transform.position - Vector2(16.0f * scale, 16.0f * scale); // Center the sprite (32x32 -> 16 offset)
                renderTransform.scale = Vector2(scale, scale);
                
                renderer->drawTexture(playerTexture, renderTransform);
            } else {
                // Fallback to orange circle if texture fails to load
                Rendering::Color playerColor(255, 200, 100, 255); // Orange
                renderer->drawCircle(transform.position, 20.0f, playerColor, true);
            }
            
            // Show player direction with a small line
            Vector2 directionEnd = transform.position + player.aimDirection * 30.0f;
            renderer->drawLine(transform.position, directionEnd, Rendering::Color::red(), 2.0f);
            
            // Show pickup range as faint green circle
            Rendering::Color rangeColor(100, 255, 100, 50);
            renderer->drawCircle(transform.position, player.pickupRange, rangeColor, false);
        }
        
        void RenderSystem::renderEnemy(EntityId entity, const Transform& transform, const Enemy& enemy) {
            Rendering::TextureHandle enemyTexture = nullptr;
            
            // Choose texture based on enemy type
            switch (enemy.type) {
                case EnemyType::SLIME:
                    enemyTexture = enemySlimeTexture;
                    break;
                case EnemyType::PEBBLIN:
                    enemyTexture = enemyPebblinTexture;
                    break;
                default:
                    // For now, use slime as default
                    enemyTexture = enemySlimeTexture;
                    break;
            }
            
            if (enemyTexture) {
                // Render enemy sprite with scaling
                float scale = 0.6f; // Slightly smaller than player
                Rendering::Transform renderTransform;
                renderTransform.position = transform.position - Vector2(16.0f * scale, 16.0f * scale); // Center the sprite
                renderTransform.scale = Vector2(scale, scale);
                
                renderer->drawTexture(enemyTexture, renderTransform);
            } else {
                // Fallback to red circle if texture fails to load
                Rendering::Color enemyColor(255, 100, 100, 255); // Red
                renderer->drawCircle(transform.position, 15.0f, enemyColor, true);
            }
        }
        
        void RenderSystem::renderProjectile(EntityId entity, const Transform& transform, const Projectile& projectile) {
            if (bulletTexture) {
                // Render bullet sprite with scaling
                float scale = 0.4f; // Small bullet
                Rendering::Transform renderTransform;
                renderTransform.position = transform.position - Vector2(8.0f * scale, 8.0f * scale); // Center the sprite
                renderTransform.scale = Vector2(scale, scale);
                
                // Rotate bullet based on direction
                renderTransform.rotation = atan2(projectile.direction.y, projectile.direction.x);
                
                renderer->drawTexture(bulletTexture, renderTransform);
            } else {
                // Fallback to small white circle if texture fails to load
                Rendering::Color bulletColor(255, 255, 255, 255); // White
                renderer->drawCircle(transform.position, 3.0f, bulletColor, true);
            }
        }
        
        void RenderSystem::renderExperienceOrb(EntityId entity, const Transform& transform, const ExperienceOrb& orb) {
            // Render experience orb as a bright green circle
            Rendering::Color orbColor(0, 255, 0, 255); // Bright green
            float radius = 8.0f;
            renderer->drawCircle(transform.position, radius, orbColor, true);
            
            // Add a small inner highlight
            Rendering::Color highlightColor(150, 255, 150, 255); // Light green
            renderer->drawCircle(transform.position, radius * 0.5f, highlightColor, true);
        }
        
        void RenderSystem::renderMaterial(EntityId entity, const Transform& transform, const Material& material) {
            // Render material as a brown/gold circle (representing materials/coins)
            Rendering::Color materialColor(255, 165, 0, 255); // Orange/gold
            float radius = 10.0f;
            renderer->drawCircle(transform.position, radius, materialColor, true);
            
            // Add a small inner highlight
            Rendering::Color highlightColor(255, 215, 0, 255); // Gold
            renderer->drawCircle(transform.position, radius * 0.6f, highlightColor, true);
        }
        
        void RenderSystem::renderSpawnIndicator(EntityId entity, const Transform& transform, const SpawnIndicator& indicator) {
            // Render flashing red X like in the original game
            float progress = indicator.getProgress();  // 0.0 to 1.0
            
            // Flashing effect: blink ~3 Hz based on elapsed time
            float blinkFrequency = 6.0f;  // blinks per second (3 Hz = 6 half-cycles per second)
            float blinkPhase = fmodf(indicator.elapsed * blinkFrequency, 2.0f);
            bool showIndicator = blinkPhase < 1.0f; // Show for first half of cycle
            
            if (showIndicator) {
                // Red color with some transparency
                int alpha = static_cast<int>(255 * (0.6f + 0.4f * sinf(indicator.elapsed * 10.0f))); // Pulsing alpha
                alpha = std::max(150, std::min(255, alpha)); // Keep between 150-255
                Rendering::Color indicatorColor(255, 50, 50, alpha); // Bright red
                
                Vector2 center = transform.position;
                float size = 14.0f; // Cross arm length (matching original game)
                
                // Draw X shape with 4 lines
                Vector2 topLeft = center + Vector2(-size, -size);
                Vector2 topRight = center + Vector2(size, -size);
                Vector2 bottomLeft = center + Vector2(-size, size);
                Vector2 bottomRight = center + Vector2(size, size);
                
                // Draw the X
                renderer->drawLine(topLeft, bottomRight, indicatorColor, 3.0f);
                renderer->drawLine(topRight, bottomLeft, indicatorColor, 3.0f);
                
                // Add a small circle at the center for more visibility
                renderer->drawCircle(center, 4.0f, indicatorColor, true);
            }
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
