#include "UISystem.h"
#include "WaveSystem.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace BrotatoGame {
    namespace Gameplay {
        
        UISystem::UISystem() 
            : playerEntity(Core::INVALID_ENTITY_ID)
        {
        }
        
        bool UISystem::initialize() {
            renderer = Core::Services::getInstance().getService<Rendering::IRenderer>();
            if (!renderer) {
                std::cout << "UISystem: Failed to get renderer service!" << std::endl;
                return false;
            }
            
            std::cout << "UISystem initialized successfully!" << std::endl;
            return true;
        }
        
        void UISystem::update(float deltaTime) {
            // UI doesn't need update logic, just rendering
        }
        
        void UISystem::render() {
            if (!entityManager || !renderer || playerEntity == Core::INVALID_ENTITY_ID) {
                return;
            }
            
            // Render all UI elements (matching original game layout)
            renderHealthBar();
            renderLevelDisplay();
            renderMaterialsCounter();
            renderWaveDisplay();
            renderWaveTimer();
            renderExperienceBar();
        }
        
        void UISystem::shutdown() {
            std::cout << "UISystem shutdown." << std::endl;
        }
        
        void UISystem::renderHealthBar() {
            auto player = entityManager->getComponent<Player>(playerEntity);
            auto health = entityManager->getComponent<Health>(playerEntity);
            
            if (!player || !health) return;
            
            // Background (dark red)
            Rendering::Rectangle bgRect(HEALTH_BAR_X, HEALTH_BAR_Y, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);
            renderer->drawRectangle(bgRect, Rendering::Color(139, 0, 0, 255), true);
            
            // Health bar (red)
            float healthPercentage = static_cast<float>(health->current) / static_cast<float>(player->maxHealth);
            int healthWidth = static_cast<int>(healthPercentage * HEALTH_BAR_WIDTH);
            Rendering::Rectangle healthRect(HEALTH_BAR_X, HEALTH_BAR_Y, healthWidth, HEALTH_BAR_HEIGHT);
            renderer->drawRectangle(healthRect, Rendering::Color(255, 0, 0, 255), true);
            
            // Border (white)
            renderer->drawRectangle(bgRect, Rendering::Color(255, 255, 255, 255), false);
            
            // Health text "X / Y"
            std::stringstream healthText;
            healthText << health->current << " / " << player->maxHealth;
            renderer->drawText(healthText.str(), Vector2(HEALTH_BAR_X + 10, HEALTH_BAR_Y + 8), Rendering::Color::white(), 16);
        }
        
        void UISystem::renderLevelDisplay() {
            auto player = entityManager->getComponent<Player>(playerEntity);
            if (!player) return;
            
            // Background (dark gray)
            Rendering::Rectangle bgRect(LEVEL_DISPLAY_X, LEVEL_DISPLAY_Y, LEVEL_DISPLAY_WIDTH, LEVEL_DISPLAY_HEIGHT);
            renderer->drawRectangle(bgRect, Rendering::Color(64, 64, 64, 255), true);
            
            // Border (white)
            renderer->drawRectangle(bgRect, Rendering::Color(255, 255, 255, 255), false);
            
            // Level text "LV.X"
            std::stringstream levelText;
            levelText << "LV." << player->level;
            renderer->drawText(levelText.str(), Vector2(LEVEL_DISPLAY_X + 10, LEVEL_DISPLAY_Y + 7), Rendering::Color::white(), 16);
        }
        
        void UISystem::renderMaterialsCounter() {
            auto player = entityManager->getComponent<Player>(playerEntity);
            if (!player) return;
            
            Vector2 center(MATERIALS_CIRCLE_X, MATERIALS_CIRCLE_Y);
            
            // Outer circle (dark green)
            drawCircle(center, MATERIALS_CIRCLE_RADIUS, Rendering::Color(0, 100, 0, 255), true);
            
            // Inner circle (bright green)
            drawCircle(center, MATERIALS_CIRCLE_RADIUS - 5, Rendering::Color(50, 200, 50, 255), true);
            
            // Materials number (centered)
            std::string materialsText = std::to_string(player->materials);
            Vector2 textPos = center;
            textPos.x -= materialsText.length() * 8; // Center for TTF text
            textPos.y -= 8;
            renderer->drawText(materialsText, textPos, Rendering::Color::white(), 20);
        }
        
        void UISystem::renderWaveDisplay() {
            if (!waveSystem) return;
            
            // Background (semi-transparent black)
            Rendering::Rectangle bgRect(WAVE_DISPLAY_X, WAVE_DISPLAY_Y, WAVE_DISPLAY_WIDTH, WAVE_DISPLAY_HEIGHT);
            renderer->drawRectangle(bgRect, Rendering::Color(0, 0, 0, 200), true);
            
            // Border (white)
            renderer->drawRectangle(bgRect, Rendering::Color(255, 255, 255, 255), false);
            
            // Wave text "WAVE X"
            std::stringstream waveText;
            waveText << "WAVE " << waveSystem->getCurrentWave();
            Vector2 textPos(WAVE_DISPLAY_X + 20, WAVE_DISPLAY_Y + 10);
            renderer->drawText(waveText.str(), textPos, Rendering::Color::white(), 20);
        }
        
        void UISystem::renderWaveTimer() {
            if (!waveSystem) return;
            
            // Background (semi-transparent black)
            Rendering::Rectangle bgRect(TIMER_DISPLAY_X, TIMER_DISPLAY_Y, TIMER_DISPLAY_WIDTH, TIMER_DISPLAY_HEIGHT);
            renderer->drawRectangle(bgRect, Rendering::Color(0, 0, 0, 200), true);
            
            // Border (white)
            renderer->drawRectangle(bgRect, Rendering::Color(255, 255, 255, 255), false);
            
            // Timer text (large numbers)
            float timeRemaining = waveSystem->getWaveTimeRemaining();
            int seconds = static_cast<int>(timeRemaining);
            if (seconds < 0) seconds = 0;
            
            std::string timerText = std::to_string(seconds);
            Vector2 textPos(TIMER_DISPLAY_X + (TIMER_DISPLAY_WIDTH/2) - (timerText.length() * 12), TIMER_DISPLAY_Y + 15);
            renderer->drawText(timerText, textPos, Rendering::Color::white(), 32); // Large font size
        }
        
        void UISystem::renderExperienceBar() {
            auto player = entityManager->getComponent<Player>(playerEntity);
            if (!player) return;
            
            // Background (dark green)
            Rendering::Rectangle bgRect(0, XP_BAR_Y, SCREEN_WIDTH, XP_BAR_HEIGHT);
            renderer->drawRectangle(bgRect, Rendering::Color(0, 100, 0, 255), true);
            
            // Experience progress (bright green)
            float xpPercentage = static_cast<float>(player->experience) / static_cast<float>(player->experienceToNextLevel);
            int xpWidth = static_cast<int>(xpPercentage * SCREEN_WIDTH);
            Rendering::Rectangle xpRect(0, XP_BAR_Y, xpWidth, XP_BAR_HEIGHT);
            renderer->drawRectangle(xpRect, Rendering::Color(0, 255, 0, 255), true);
        }
        
        void UISystem::renderBitmapText(const std::string& text, const Vector2& position, const Rendering::Color& color, int scale) {
            // Simple bitmap text using renderer's drawBitmapText method
            renderer->drawBitmapText(text, position, color, scale);
        }
        
        void UISystem::renderBitmapNumber(int number, const Vector2& position, const Rendering::Color& color, int scale) {
            std::string numberText = std::to_string(number);
            renderBitmapText(numberText, position, color, scale);
        }
        
        void UISystem::drawCircle(const Vector2& center, float radius, const Rendering::Color& color, bool filled) {
            renderer->drawCircle(center, radius, color, filled);
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
