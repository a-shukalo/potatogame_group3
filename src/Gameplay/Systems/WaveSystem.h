#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Core/Events/EventManager.h"
#include "../Entities/EntityManager.h"

namespace BrotatoGame {
    namespace Gameplay {
        
        enum class WaveState {
            PREPARING,  // Between waves (shop time)
            ACTIVE,     // Wave is running
            COMPLETED   // Wave just finished
        };
        
        class WaveSystem : public Core::ISystem {
        public:
            WaveSystem();
            ~WaveSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            
            // Wave management
            void startWave();
            void endWave();
            void startNextWave();
            
            // Getters
            int getCurrentWave() const { return currentWave; }
            WaveState getWaveState() const { return waveState; }
            float getWaveTimeRemaining() const { return waveDuration - waveTimer; }
            float getWaveDuration() const { return waveDuration; }
            bool isWaveActive() const { return waveState == WaveState::ACTIVE; }
            
            // Shop integration
            bool isShopTime() const { return waveState == WaveState::PREPARING; }
            void closeShop() { startWave(); } // Called when player closes shop
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<Core::EventManager> eventManager;
            
            // Wave state
            WaveState waveState;
            int currentWave;
            float waveTimer;
            float waveDuration;
            
            // Wave progression
            void calculateWaveDuration();
            void onWaveStart();
            void onWaveEnd();
            
            // Constants from original game
            static const float INITIAL_WAVE_DURATION; // 20 seconds
            static const float WAVE_DURATION_INCREMENT; // 5 seconds per wave
            static const float MAX_WAVE_DURATION; // 60 seconds maximum
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
