#pragma once
#include "../../Core/Math/Vector2.h"
#include "../../Core/Events/GameEvents.h"
#include <bitset>
#include <array>
#include <iostream>

namespace BrotatoGame {
    namespace Gameplay {
        
        using Vector2 = Core::Vector2;
        using EntityId = Core::EntityId;
        using WeaponType = Core::WeaponType;
        enum class EnemyType {
            BASE,
            BASIC,
            FAST,
            STRONG,
            SLIME,
            PEBBLIN
        };
        
        // Maximum number of components per entity
        const size_t MAX_COMPONENTS = 32;
        
        // Component type IDs (assigned automatically)
        using ComponentTypeId = size_t;
        
        // Component type ID counter
        inline ComponentTypeId getNextComponentTypeId() {
            static ComponentTypeId counter = 0;
            return counter++;
        }
        
        template<typename T>
        ComponentTypeId getComponentTypeId() {
            static ComponentTypeId typeId = getNextComponentTypeId();
            return typeId;
        }
        
        // Component signature for entities
        using ComponentSignature = std::bitset<MAX_COMPONENTS>;
        
        // =============================================================================
        // Core Components
        // =============================================================================
        
        struct Transform {
            Vector2 position{0, 0};
            float rotation = 0.0f;
            Vector2 scale{1, 1};
            
            Transform() = default;
            Transform(const Vector2& pos, float rot = 0.0f, const Vector2& scl = Vector2(1, 1))
                : position(pos), rotation(rot), scale(scl) {}
        };
        
        struct Health {
            int current = 100;
            int maximum = 100;
            float regeneration = 0.0f;
            float regenTimer = 0.0f;
            bool invulnerable = false;
            float invulnerabilityTime = 0.0f;
            
            Health() = default;
            Health(int hp, float regen = 0.0f) : current(hp), maximum(hp), regeneration(regen) {}
            
            bool isAlive() const { return current > 0; }
            void takeDamage(int amount) {
                if (!invulnerable) {
                    current = std::max(0, current - amount);
                }
            }
            void heal(int amount) {
                current = std::min(maximum, current + amount);
            }
        };
        
        struct Movement {
            Vector2 velocity{0, 0};
            float speed = 200.0f;
            float acceleration = 1000.0f;
            float friction = 0.9f;
            float maxSpeed = 500.0f;
            
            Movement() = default;
            Movement(float spd) : speed(spd), maxSpeed(spd * 2.0f) {}
        };
        
        struct Collider {
            float radius = 20.0f;
            bool isTrigger = false;
            
            Collider() = default;
            Collider(float r, bool trigger = false) : radius(r), isTrigger(trigger) {}
        };
        
        // =============================================================================
        // Weapon Components
        // =============================================================================
        
        enum class WeaponTier {
            TIER_1 = 1,
            TIER_2 = 2,
            TIER_3 = 3,
            TIER_4 = 4
        };
        
        struct Weapon {
            Core::WeaponType type = Core::WeaponType::PISTOL;
            WeaponTier tier = WeaponTier::TIER_1;
            int damage = 10;
            float fireRate = 1.0f;        // Shots per second
            float range = 400.0f;
            float critChance = 0.05f;     // 5% base crit chance
            float critMultiplier = 2.0f;
            float lastShotTime = 0.0f;
            bool isCharging = false;      // For railgun
            float chargeTime = 0.0f;
            float maxChargeTime = 0.0f;   // Max charge time for railgun
            int pelletsPerShot = 1;       // For shotgun
            float spread = 0.0f;          // Spread angle for shotgun
            bool isChargeWeapon = false;  // For railgun
            
            Weapon() = default;
            Weapon(Core::WeaponType weaponType, int dmg, float rate) 
                : type(weaponType), damage(dmg), fireRate(rate) {}
            Weapon(Core::WeaponType weaponType, WeaponTier wTier, int dmg, float rate, float rng) 
                : type(weaponType), tier(wTier), damage(dmg), fireRate(rate), range(rng) {}
            
            bool canFire(float currentTime) const { 
                return (currentTime - lastShotTime) >= (1.0f / fireRate);
            }
            
            void updateCharge(float deltaTime) {
                if (isChargeWeapon && isCharging) {
                    chargeTime = std::min(chargeTime + deltaTime, maxChargeTime);
                }
            }
            
            float getChargePercent() const {
                return isChargeWeapon ? (chargeTime / maxChargeTime) : 1.0f;
            }
            
            bool isFullyCharged() const {
                return !isChargeWeapon || chargeTime >= maxChargeTime;
            }
        };
        
        struct Projectile {
            Vector2 direction{1, 0};
            float speed = 400.0f;
            int damage = 10;
            float maxRange = 400.0f;
            float travelDistance = 0.0f;
            float lifetime = 1.5f;       // Time-based expiration as backup
            bool enemyOwned = false;
            int pierceCount = 0;         // How many enemies it can pierce through
            bool explosive = false;
            float explosionRadius = 0.0f;
            
            Projectile() = default;
            Projectile(const Vector2& dir, float spd, int dmg, float range) 
                : direction(dir), speed(spd), damage(dmg), maxRange(range), lifetime(range / spd) {}
            
            bool isExpired() const { return travelDistance >= maxRange || lifetime <= 0.0f; }
        };
        
        // =============================================================================
        // AI Components
        // =============================================================================
        
        enum class AIState {
            IDLE,
            CHASE,
            ATTACK,
            RETREAT,
            DEAD
        };
        
        struct AI {
            AIState state = AIState::IDLE;
            EntityId targetId = Core::INVALID_ENTITY_ID;
            float aggroRange = 200.0f;
            float attackRange = 50.0f;
            float lastActionTime = 0.0f;
            float actionCooldown = 1.0f;
            Vector2 wanderTarget{0, 0};
            float stateTimer = 0.0f;
            
            AI() = default;
            AI(float aggro, float attack) : aggroRange(aggro), attackRange(attack) {}
        };
        
        // =============================================================================
        // Player Components
        // =============================================================================
        
        struct Player {
            // Progression
            int level = 1;
            int experience = 0;
            int experienceToNextLevel = 10; // Starting XP requirement
            int materials = 0; // Start with 0 materials (earn from enemies)
            
            // Stats (Brotato-style)
            int maxHealth = 100;
            float moveSpeed = 200.0f;
            float pickupRange = 80.0f;
            float attackSpeed = 1.0f;
            int damage = 10;
            float range = 400.0f;
            int armor = 0;
            float healthRegen = 0.0f;
            float dodgeChance = 0.0f;
            int luck = 0;
            
            // Player state  
            bool isDashing = false;
            float dashCooldown = 0.0f;
            Vector2 aimDirection{1, 0};
            
            Player() = default;
            
            // Level up bonuses from original game
            void levelUp() {
                level++;
                maxHealth += 5;      // +5 max health per level
                damage += 1;         // +1 damage per level  
                moveSpeed += 2.0f;   // +2 move speed per level
                
                // Calculate next level requirement: 10 + (level-1) × 15 + (level-1)² × 5
                int levelIndex = level - 1;
                experienceToNextLevel = 10 + (levelIndex * 15) + (levelIndex * levelIndex * 5);
                
                std::cout << "LEVEL UP! Now level " << level << std::endl;
                std::cout << "Bonuses: +5 max HP, +1 damage, +2 move speed" << std::endl;
                std::cout << "Next level requires: " << experienceToNextLevel << " XP" << std::endl;
            }
            
            bool canLevelUp() const {
                return experience >= experienceToNextLevel;
            }
            
            void gainExperience(int xp) {
                experience += xp;
                std::cout << "Gained " << xp << " XP (Total: " << experience << "/" << experienceToNextLevel << ")" << std::endl;
            }
            
            void gainMaterials(int mats) {
                materials += mats;
                std::cout << "Gained " << mats << " materials (Total: " << materials << ")" << std::endl;
            }
            
            int getExperienceToNextLevel() const {
                int nextLevel = level + 1;
                return (nextLevel + 3) * (nextLevel + 3);
            }
        };
        
        struct Inventory {
            std::array<EntityId, 6> weaponSlots{};  // Max 6 weapons like Brotato
            int activeWeapons = 0;
            
            Inventory() {
                weaponSlots.fill(Core::INVALID_ENTITY_ID);
            }
            
            bool hasSpace() const { return activeWeapons < 6; }
            
            bool addWeapon(EntityId weaponId) {
                if (hasSpace()) {
                    for (size_t i = 0; i < weaponSlots.size(); ++i) {
                        if (weaponSlots[i] == Core::INVALID_ENTITY_ID) {
                            weaponSlots[i] = weaponId;
                            activeWeapons++;
                            return true;
                        }
                    }
                }
                return false;
            }
            
            bool removeWeapon(EntityId weaponId) {
                for (size_t i = 0; i < weaponSlots.size(); ++i) {
                    if (weaponSlots[i] == weaponId) {
                        weaponSlots[i] = Core::INVALID_ENTITY_ID;
                        activeWeapons--;
                        return true;
                    }
                }
                return false;
            }
        };
        
        // =============================================================================
        // Enemy Components
        // =============================================================================
        
        struct Enemy {
            EnemyType type = EnemyType::BASE;
            int maxHealth = 30;
            int currentHealth = 30;
            int damage = 10;
            float speed = 50.0f;
            int experienceValue = 5;
            int materialValue = 2;
            int scoreValue = 10;
            float spawnTime = 0.0f;
            bool canSplit = false;          // For splitter enemies
            int splitCount = 2;
            
            Enemy() = default;
            Enemy(EnemyType enemyType, int score) : type(enemyType), scoreValue(score) {}
        };
        
        // =============================================================================
        // Effect Components
        // =============================================================================
        
        struct Lifetime {
            float timeRemaining;
            bool destroyOnExpire = true;
            
            Lifetime(float time) : timeRemaining(time) {}
            
            bool isExpired() const { return timeRemaining <= 0.0f; }
            void update(float deltaTime) { timeRemaining -= deltaTime; }
        };
        
        struct Pickup {
            enum Type {
                EXPERIENCE,
                MATERIAL,
                HEALTH
            };
            
            Type type = EXPERIENCE;
            int value = 1;
            float attractionRange = 100.0f;
            bool isBeingAttracted = false;
            Vector2 attractionTarget{0, 0};
            
            Pickup(Type pickupType, int val) : type(pickupType), value(val) {}
        };
        
        struct ExperienceOrb {
            int experienceValue;
            float lifetime;
            bool collected;
            float magnetRadius; // Radius at which orb moves toward player
            
            ExperienceOrb(int xp = 1) 
                : experienceValue(xp)
                , lifetime(30.0f)
                , collected(false) 
                , magnetRadius(100.0f) {}
            
            bool isExpired() const { return lifetime <= 0.0f || collected; }
        };
        
        struct Material {
            int materialValue;
            int experienceValue; // Materials also give a small amount of XP
            float lifetime;
            bool collected;
            float magnetRadius;
            
            Material(int materials = 2, int xp = 1) 
                : materialValue(materials)
                , experienceValue(xp)
                , lifetime(30.0f)
                , collected(false)
                , magnetRadius(100.0f) {}
            
            bool isExpired() const { return lifetime <= 0.0f || collected; }
        };
        
        struct VisualEffect {
            float animationTime = 0.0f;
            float maxAnimationTime = 1.0f;
            bool loop = false;
            
            VisualEffect(float maxTime, bool looping = false) 
                : maxAnimationTime(maxTime), loop(looping) {}
            
            float getProgress() const { 
                return maxAnimationTime > 0 ? animationTime / maxAnimationTime : 1.0f; 
            }
            
            bool isFinished() const { 
                return !loop && animationTime >= maxAnimationTime; 
            }
        };
        
        struct SpawnIndicator {
            EnemyType enemyType;
            float elapsed;
            float duration; // Total duration (2 seconds like original)
            bool completed;
            
            SpawnIndicator(EnemyType type = EnemyType::BASIC, float dur = 2.0f) 
                : enemyType(type), elapsed(0.0f), duration(dur), completed(false) {}
            
            bool isComplete() const { return elapsed >= duration; }
            
            float getProgress() const { return elapsed / duration; } // 0.0 to 1.0
            
            void update(float deltaTime) { 
                elapsed += deltaTime;
                if (elapsed >= duration) completed = true;
            }
        };
        
        // =============================================================================
        // Rendering Components
        // =============================================================================
        
        struct Sprite {
            std::string texturePath;
            Vector2 size{32, 32};
            Vector2 offset{0, 0};
            float alpha = 1.0f;
            bool visible = true;
            
            Sprite() = default;
            Sprite(const std::string& path, const Vector2& spriteSize) 
                : texturePath(path), size(spriteSize) {}
        };
        
        struct AnimatedSprite {
            std::string texturePath;
            Vector2 frameSize{32, 32};
            int frameCount = 1;
            int currentFrame = 0;
            float frameTime = 0.1f;
            float currentFrameTime = 0.0f;
            bool loop = true;
            bool playing = true;
            
            AnimatedSprite() = default;
            AnimatedSprite(const std::string& path, const Vector2& fSize, int frames, float fTime) 
                : texturePath(path), frameSize(fSize), frameCount(frames), frameTime(fTime) {}
        };
        

        
    } // namespace Gameplay
} // namespace BrotatoGame
