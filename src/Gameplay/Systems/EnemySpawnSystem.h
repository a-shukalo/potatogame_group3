#pragma once
#include "../../Core/Engine/Application.h"
#include "../Entities/EntityManager.h"
#include "../Components/Components.h"
#include <random>

namespace BrotatoGame {
    namespace Gameplay {
        
        // Forward declaration
        class WaveSystem;
        
        class EnemySpawnSystem : public Core::ISystem {
        public:
            EnemySpawnSystem();
            ~EnemySpawnSystem() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            void setEntityManager(std::shared_ptr<EntityManager> em) { entityManager = em; }
            void setPlayerEntity(EntityId player) { playerEntity = player; }
            void setWaveSystem(std::shared_ptr<WaveSystem> ws) { waveSystem = ws; }
            
        private:
            std::shared_ptr<EntityManager> entityManager;
            std::shared_ptr<WaveSystem> waveSystem;
            EntityId playerEntity;
            
            // Spawn timing
            float spawnTimer;
            float spawnInterval; // seconds between spawns
            int maxEnemies;
            int currentEnemyCount;
            
            // Spawn indicators (from original game)
            static const float SPAWN_TELEGRAPH_DURATION; // 2 seconds warning
            
            // Random generation
            std::mt19937 randomGenerator;
            std::uniform_real_distribution<float> angleDistribution;
            std::uniform_real_distribution<float> distanceDistribution;
            
            void spawnEnemy();
            Vector2 getRandomSpawnPosition();
            int countCurrentEnemies();
            
            // Enemy creation methods
            
            void createBasicEnemy(const Vector2& position);
            void createFastEnemy(const Vector2& position);
            void createStrongEnemy(const Vector2& position);
            void createSlimeEnemy(const Vector2& position);
            void createPebblinEnemy(const Vector2& position);
            
            // Spawn indicator methods
            void createSpawnIndicator(const Vector2& position, EnemyType enemyType);
            void updateSpawnIndicators(float deltaTime);
            void processCompletedIndicators();
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
