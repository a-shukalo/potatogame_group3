#pragma once
#include "../../Core/Engine/Application.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"
#include "../../Rendering/Renderer/IRenderer.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        // Forward declarations
        class WaveSystem;
        
        class UISystem : public Core::ISystem {
        public:
            UISystem();
            ~UISystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void render() override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            void setPlayerEntity(EntityId player) { playerEntity = player; }
            void setWaveSystem(std::shared_ptr<WaveSystem> ws) { waveSystem = ws; }
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<Rendering::IRenderer> renderer;
            std::shared_ptr<WaveSystem> waveSystem;
            EntityId playerEntity;
            
            // UI rendering methods
            void renderHealthBar();
            void renderLevelDisplay();
            void renderMaterialsCounter();
            void renderWaveDisplay();
            void renderWaveTimer();
            void renderExperienceBar();
            void renderPlayerStats();
            
            // Helper methods
            void renderBitmapText(const std::string& text, const Vector2& position, const Rendering::Color& color, int scale = 1);
            void renderBitmapNumber(int number, const Vector2& position, const Rendering::Color& color, int scale = 1);
            void drawCircle(const Vector2& center, float radius, const Rendering::Color& color, bool filled = true);
            
            // UI layout constants (matching original game)
            static const int SCREEN_WIDTH = 1920;
            static const int SCREEN_HEIGHT = 1080;
            
            // UI positions (matching original)
            static const int HEALTH_BAR_X = 20;
            static const int HEALTH_BAR_Y = 20;
            static const int HEALTH_BAR_WIDTH = 200;
            static const int HEALTH_BAR_HEIGHT = 35;
            
            static const int LEVEL_DISPLAY_X = 20;
            static const int LEVEL_DISPLAY_Y = 65;
            static const int LEVEL_DISPLAY_WIDTH = 120;
            static const int LEVEL_DISPLAY_HEIGHT = 30;
            
            static const int MATERIALS_CIRCLE_X = 70;
            static const int MATERIALS_CIRCLE_Y = 150;
            static const int MATERIALS_CIRCLE_RADIUS = 40;
            
            static const int WAVE_DISPLAY_X = SCREEN_WIDTH / 2 - 80;
            static const int WAVE_DISPLAY_Y = 20;
            static const int WAVE_DISPLAY_WIDTH = 160;
            static const int WAVE_DISPLAY_HEIGHT = 40;
            
            static const int TIMER_DISPLAY_X = SCREEN_WIDTH / 2 - 60;
            static const int TIMER_DISPLAY_Y = 70;
            static const int TIMER_DISPLAY_WIDTH = 120;
            static const int TIMER_DISPLAY_HEIGHT = 60;
            
            static const int XP_BAR_Y = SCREEN_HEIGHT - 15;
            static const int XP_BAR_HEIGHT = 15;
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
