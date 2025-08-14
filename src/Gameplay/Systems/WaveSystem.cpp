#include "WaveSystem.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>

namespace BrotatoGame {
    namespace Gameplay {
        
        // Constants from original game
        const float WaveSystem::INITIAL_WAVE_DURATION = 20.0f;  // 20 seconds
        const float WaveSystem::WAVE_DURATION_INCREMENT = 5.0f; // +5 seconds per wave
        const float WaveSystem::MAX_WAVE_DURATION = 60.0f;      // 60 seconds maximum
        
        WaveSystem::WaveSystem() 
            : waveState(WaveState::PREPARING)
            , currentWave(1)
            , waveTimer(0.0f)
            , waveDuration(INITIAL_WAVE_DURATION)
        {
        }
        
        bool WaveSystem::initialize() {
            eventManager = Core::Services::getInstance().getService<Core::EventManager>();
            if (!eventManager) {
                std::cout << "WaveSystem: Failed to get event manager service!" << std::endl;
                return false;
            }
            
            std::cout << "WaveSystem initialized successfully!" << std::endl;
            std::cout << "Wave 1 preparation phase - waiting for shop to close..." << std::endl;
            
            return true;
        }
        
        void WaveSystem::update(float deltaTime) {
            switch (waveState) {
                case WaveState::PREPARING:
                    // Waiting for shop to close - no timer update
                    break;
                    
                case WaveState::ACTIVE:
                    waveTimer += deltaTime;
                    
                    // Check if wave is completed
                    if (waveTimer >= waveDuration) {
                        endWave();
                    }
                    break;
                    
                case WaveState::COMPLETED:
                    // Briefly completed, should transition to next wave prep
                    startNextWave();
                    break;
            }
        }
        
        void WaveSystem::shutdown() {
            std::cout << "WaveSystem shutdown." << std::endl;
        }
        
        void WaveSystem::startWave() {
            if (waveState != WaveState::PREPARING) {
                std::cout << "WaveSystem: Cannot start wave - not in preparation phase!" << std::endl;
                return;
            }
            
            waveState = WaveState::ACTIVE;
            waveTimer = 0.0f;
            
            onWaveStart();
        }
        
        void WaveSystem::endWave() {
            if (waveState != WaveState::ACTIVE) {
                std::cout << "WaveSystem: Cannot end wave - wave not active!" << std::endl;
                return;
            }
            
            waveState = WaveState::COMPLETED;
            
            onWaveEnd();
        }
        
        void WaveSystem::startNextWave() {
            if (waveState != WaveState::COMPLETED) {
                std::cout << "WaveSystem: Cannot start next wave - previous wave not completed!" << std::endl;
                return;
            }
            
            currentWave++;
            calculateWaveDuration();
            waveState = WaveState::PREPARING;
            
            std::cout << "Wave " << currentWave << " preparation phase - waiting for shop to close..." << std::endl;
            std::cout << "Next wave duration: " << waveDuration << " seconds" << std::endl;
        }
        
        void WaveSystem::calculateWaveDuration() {
            // Original game formula: 20s, 25s, 30s, 35s, ..., up to 60s max
            waveDuration = INITIAL_WAVE_DURATION + (currentWave - 1) * WAVE_DURATION_INCREMENT;
            
            // Cap at maximum duration
            if (waveDuration > MAX_WAVE_DURATION) {
                waveDuration = MAX_WAVE_DURATION;
            }
        }
        
        void WaveSystem::onWaveStart() {
            std::cout << "=== WAVE " << currentWave << " STARTED ===" << std::endl;
            std::cout << "Duration: " << waveDuration << " seconds" << std::endl;
            std::cout << "Survive and collect experience & materials!" << std::endl;
            
            // TODO: Publish WaveStartEvent for other systems to react
            // if (eventManager) {
            //     eventManager->publish(WaveStartEvent(currentWave, waveDuration));
            // }
        }
        
        void WaveSystem::onWaveEnd() {
            std::cout << "=== WAVE " << currentWave << " COMPLETED ===" << std::endl;
            std::cout << "Opening shop for upgrades..." << std::endl;
            
            // TODO: Publish WaveEndEvent for other systems to react
            // if (eventManager) {
            //     eventManager->publish(WaveEndEvent(currentWave));
            // }
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
