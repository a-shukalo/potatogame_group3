#include "EnemySpawnSystem.h"
#include "WaveSystem.h"
#include <iostream>
#include <cmath>
#include <random>

namespace BrotatoGame {
    namespace Gameplay {
        
        // Constants
        const float EnemySpawnSystem::SPAWN_TELEGRAPH_DURATION = 2.0f; // 2 seconds warning like original
        
        EnemySpawnSystem::EnemySpawnSystem() 
            : playerEntity(Core::INVALID_ENTITY_ID)
            , spawnTimer(0.0f)
            , spawnInterval(2.0f) // spawn every 2 seconds
            , maxEnemies(15)
            , currentEnemyCount(0)
            , randomGenerator(std::random_device{}())
            , angleDistribution(0.0f, 2.0f * 3.14159f)
            , distanceDistribution(400.0f, 600.0f) // spawn 400-600 pixels from player
        {
        }
        
        bool EnemySpawnSystem::initialize() {
            std::cout << "EnemySpawnSystem initialized successfully!" << std::endl;
            return true;
        }
        
        void EnemySpawnSystem::update(float deltaTime) {
            if (!entityManager || playerEntity == Core::INVALID_ENTITY_ID) {
                return;
            }
            
            // Only spawn enemies during active waves
            if (!waveSystem || !waveSystem->isWaveActive()) {
                return; // No spawning during shop/preparation time
            }
            
            // Update spawn indicators and process completed ones
            updateSpawnIndicators(deltaTime);
            processCompletedIndicators();
            
            // Update spawn timer
            spawnTimer += deltaTime;
            
            // Count current enemies
            currentEnemyCount = countCurrentEnemies();
            
            // Create spawn indicator instead of directly spawning enemy
            if (spawnTimer >= spawnInterval && currentEnemyCount < maxEnemies) {
                Vector2 spawnPos = getRandomSpawnPosition();
                
                // Randomly choose enemy type
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dist(0, 2); // 0: BASIC, 1: SLIME, 2: PEBBLIN
                
                EnemyType enemyType;
                switch (dist(gen)) {
                    case 0: enemyType = EnemyType::BASIC; break;
                    case 1: enemyType = EnemyType::SLIME; break;
                    case 2: enemyType = EnemyType::PEBBLIN; break;
                    default: enemyType = EnemyType::BASIC; break;
                }
                
                // Create spawn indicator instead of enemy
                createSpawnIndicator(spawnPos, enemyType);
                spawnTimer = 0.0f;
                
                // Scale difficulty based on current wave
                float waveNumber = static_cast<float>(waveSystem->getCurrentWave());
                float waveSpeedMultiplier = 1.0f + (waveNumber - 1) * 0.1f; // 10% faster spawning per wave
                
                // Increase spawn rate over time (make game harder)
                if (spawnInterval > 0.5f) {
                    spawnInterval -= 0.01f * waveSpeedMultiplier;
                }
            }
        }
        
        void EnemySpawnSystem::shutdown() {
            std::cout << "EnemySpawnSystem shutdown." << std::endl;
        }
        
        void EnemySpawnSystem::spawnEnemy() {
            Vector2 spawnPos = getRandomSpawnPosition();
            
            // Randomly choose enemy type
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 2);
            
            int enemyTypeRoll = dist(gen);
            switch (enemyTypeRoll) {
                case 0:
                    createSlimeEnemy(spawnPos);
                    std::cout << "Spawned SLIME enemy at (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
                    break;
                case 1:
                    createPebblinEnemy(spawnPos);
                    std::cout << "Spawned PEBBLIN enemy at (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
                    break;
                default:
                    createBasicEnemy(spawnPos);
                    std::cout << "Spawned BASIC enemy at (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
                    break;
            }
        }
        
        Vector2 EnemySpawnSystem::getRandomSpawnPosition() {
            // Get player position
            auto playerTransform = entityManager->getComponent<Transform>(playerEntity);
            if (!playerTransform) {
                return Vector2(960, 540); // fallback to center
            }
            
            // Generate random angle and distance
            float angle = angleDistribution(randomGenerator);
            float distance = distanceDistribution(randomGenerator);
            
            // Calculate spawn position around player
            Vector2 spawnPos;
            spawnPos.x = playerTransform->position.x + cos(angle) * distance;
            spawnPos.y = playerTransform->position.y + sin(angle) * distance;
            
            // Ensure spawn position is not too close to screen edges
            spawnPos.x = std::max(50.0f, std::min(1870.0f, spawnPos.x));
            spawnPos.y = std::max(50.0f, std::min(1030.0f, spawnPos.y));
            
            return spawnPos;
        }
        
        int EnemySpawnSystem::countCurrentEnemies() {
            auto enemies = entityManager->getEntitiesWith<Transform, Enemy>();
            return static_cast<int>(enemies.size());
        }
        
        void EnemySpawnSystem::createBasicEnemy(const Vector2& position) {
            EntityId enemyId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(enemyId, Transform(position));
            
            // Add Enemy component
            Enemy enemyComponent;
            enemyComponent.type = EnemyType::BASIC;
            enemyComponent.maxHealth = 30;
            enemyComponent.currentHealth = 30;
            enemyComponent.damage = 10;
            enemyComponent.speed = 50.0f; // slower than player
            enemyComponent.experienceValue = 5;
            enemyComponent.materialValue = 2;
            entityManager->addComponent<Enemy>(enemyId, enemyComponent);
            
            // Add Health component
            Health enemyHealth(30);
            entityManager->addComponent<Health>(enemyId, enemyHealth);
            
            // Add Movement component
            Movement enemyMovement(50.0f); // 50 speed - slower than player
            entityManager->addComponent<Movement>(enemyId, enemyMovement);
        }
        
        void EnemySpawnSystem::createFastEnemy(const Vector2& position) {
            EntityId enemyId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(enemyId, Transform(position));
            
            // Add Enemy component
            Enemy enemyComponent;
            enemyComponent.type = EnemyType::FAST;
            enemyComponent.maxHealth = 15;
            enemyComponent.currentHealth = 15;
            enemyComponent.damage = 8;
            enemyComponent.speed = 120.0f; // faster than player
            enemyComponent.experienceValue = 8;
            enemyComponent.materialValue = 3;
            entityManager->addComponent<Enemy>(enemyId, enemyComponent);
            
            // Add Health component
            Health enemyHealth(15);
            entityManager->addComponent<Health>(enemyId, enemyHealth);
            
            // Add Movement component
            Movement enemyMovement(120.0f);
            entityManager->addComponent<Movement>(enemyId, enemyMovement);
        }
        
        void EnemySpawnSystem::createStrongEnemy(const Vector2& position) {
            EntityId enemyId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(enemyId, Transform(position));
            
            // Add Enemy component
            Enemy enemyComponent;
            enemyComponent.type = EnemyType::STRONG;
            enemyComponent.maxHealth = 80;
            enemyComponent.currentHealth = 80;
            enemyComponent.damage = 20;
            enemyComponent.speed = 30.0f; // very slow but tanky
            enemyComponent.experienceValue = 15;
            enemyComponent.materialValue = 5;
            entityManager->addComponent<Enemy>(enemyId, enemyComponent);
            
            // Add Health component
            Health enemyHealth(80);
            entityManager->addComponent<Health>(enemyId, enemyHealth);
            
            // Add Movement component
            Movement enemyMovement(30.0f);
            entityManager->addComponent<Movement>(enemyId, enemyMovement);
        }
        
        void EnemySpawnSystem::createSlimeEnemy(const Vector2& position) {
            EntityId enemyId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(enemyId, Transform(position));
            
            // Add Enemy component
            Enemy enemyComponent;
            enemyComponent.type = EnemyType::SLIME;
            enemyComponent.maxHealth = 50;
            enemyComponent.currentHealth = 50;
            enemyComponent.damage = 15;
            enemyComponent.speed = 40.0f;
            enemyComponent.experienceValue = 8;
            enemyComponent.materialValue = 3;
            enemyComponent.scoreValue = 15;
            entityManager->addComponent<Enemy>(enemyId, enemyComponent);
            
            // Add Health component
            Health enemyHealth(50);
            entityManager->addComponent<Health>(enemyId, enemyHealth);
            
            // Add Movement component
            Movement enemyMovement(40.0f);
            entityManager->addComponent<Movement>(enemyId, enemyMovement);
        }
        
        void EnemySpawnSystem::createPebblinEnemy(const Vector2& position) {
            EntityId enemyId = entityManager->createEntity();
            
            // Add Transform component
            entityManager->addComponent<Transform>(enemyId, Transform(position));
            
            // Add Enemy component
            Enemy enemyComponent;
            enemyComponent.type = EnemyType::PEBBLIN;
            enemyComponent.maxHealth = 20;
            enemyComponent.currentHealth = 20;
            enemyComponent.damage = 8;
            enemyComponent.speed = 70.0f;
            enemyComponent.experienceValue = 6;
            enemyComponent.materialValue = 2;
            enemyComponent.scoreValue = 12;
            entityManager->addComponent<Enemy>(enemyId, enemyComponent);
            
            // Add Health component
            Health enemyHealth(20);
            entityManager->addComponent<Health>(enemyId, enemyHealth);
            
            // Add Movement component
            Movement enemyMovement(70.0f);
            entityManager->addComponent<Movement>(enemyId, enemyMovement);
        }
        
        void EnemySpawnSystem::createSpawnIndicator(const Vector2& position, EnemyType enemyType) {
            EntityId indicatorId = entityManager->createEntity();
            entityManager->addComponent<Transform>(indicatorId, Transform(position));
            entityManager->addComponent<SpawnIndicator>(indicatorId, SpawnIndicator(enemyType, SPAWN_TELEGRAPH_DURATION));
            
            std::cout << "Created spawn indicator for enemy type at (" << position.x << ", " << position.y << ")" << std::endl;
        }
        
        void EnemySpawnSystem::updateSpawnIndicators(float deltaTime) {
            auto indicators = entityManager->getEntitiesWith<Transform, SpawnIndicator>();
            
            for (EntityId indicatorId : indicators) {
                auto indicator = entityManager->getComponent<SpawnIndicator>(indicatorId);
                if (indicator && !indicator->completed) {
                    indicator->update(deltaTime);
                }
            }
        }
        
        void EnemySpawnSystem::processCompletedIndicators() {
            auto indicators = entityManager->getEntitiesWith<Transform, SpawnIndicator>();
            
            for (EntityId indicatorId : indicators) {
                auto transform = entityManager->getComponent<Transform>(indicatorId);
                auto indicator = entityManager->getComponent<SpawnIndicator>(indicatorId);
                
                if (transform && indicator && indicator->isComplete()) {
                    Vector2 spawnPosition = transform->position;
                    EnemyType enemyType = indicator->enemyType;
                    
                    // Spawn the actual enemy
                    switch (enemyType) {
                        case EnemyType::SLIME:
                            createSlimeEnemy(spawnPosition);
                            std::cout << "Spawned SLIME enemy at (" << spawnPosition.x << ", " << spawnPosition.y << ")" << std::endl;
                            break;
                        case EnemyType::PEBBLIN:
                            createPebblinEnemy(spawnPosition);
                            std::cout << "Spawned PEBBLIN enemy at (" << spawnPosition.x << ", " << spawnPosition.y << ")" << std::endl;
                            break;
                        case EnemyType::BASIC:
                        default:
                            createBasicEnemy(spawnPosition);
                            std::cout << "Spawned BASIC enemy at (" << spawnPosition.x << ", " << spawnPosition.y << ")" << std::endl;
                            break;
                    }
                    
                    // Remove the indicator
                    entityManager->destroyEntity(indicatorId);
                }
            }
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
