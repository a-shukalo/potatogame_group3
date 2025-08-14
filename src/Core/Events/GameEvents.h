#pragma once
#include "EventManager.h"
#include "../Math/Vector2.h"
#include <string>

namespace BrotatoGame {
    namespace Core {
        
        // Entity ID type for ECS system
        using EntityId = uint32_t;
        const EntityId INVALID_ENTITY_ID = 0;
        
        // Weapon and enemy type enums (shared across modules)
        enum class WeaponType {
            PISTOL,
            SMG,
            SHOTGUN,
            RAILGUN,
            MELEE_STICK
        };
        
        enum class EnemyType {
            BASE,
            SLIME,
            PEBBLIN
        };
        
        // =============================================================================
        // Core Game Events
        // =============================================================================
        
        struct EntityCreatedEvent : public IEvent {
            EntityId entityId;
            std::string entityType;
            
            EntityCreatedEvent(EntityId id, const std::string& type) 
                : entityId(id), entityType(type) {}
        };
        
        struct EntityDestroyedEvent : public IEvent {
            EntityId entityId;
            
            EntityDestroyedEvent(EntityId id) : entityId(id) {}
        };
        
        // =============================================================================
        // Player Events
        // =============================================================================
        
        struct PlayerLevelUpEvent : public IEvent {
            EntityId playerId;
            int newLevel;
            int healthIncrease;
            
            PlayerLevelUpEvent(EntityId id, int level, int health) 
                : playerId(id), newLevel(level), healthIncrease(health) {}
        };
        
        struct PlayerHealthChangedEvent : public IEvent {
            EntityId playerId;
            int currentHealth;
            int maxHealth;
            int damageDealt; // Positive for damage taken, negative for healing
            
            PlayerHealthChangedEvent(EntityId id, int current, int max, int damage) 
                : playerId(id), currentHealth(current), maxHealth(max), damageDealt(damage) {}
        };
        
        struct PlayerMaterialsChangedEvent : public IEvent {
            EntityId playerId;
            int currentMaterials;
            int changeAmount;
            
            PlayerMaterialsChangedEvent(EntityId id, int current, int change) 
                : playerId(id), currentMaterials(current), changeAmount(change) {}
        };
        
        // =============================================================================
        // Combat Events
        // =============================================================================
        
        struct WeaponFireEvent : public IEvent {
            EntityId weaponId;
            EntityId ownerId;
            Vector2 position;
            Vector2 direction;
            WeaponType weaponType;
            int damage;
            
            WeaponFireEvent(EntityId weapon, EntityId owner, const Vector2& pos, const Vector2& dir, WeaponType type, int dmg) 
                : weaponId(weapon), ownerId(owner), position(pos), direction(dir), weaponType(type), damage(dmg) {}
        };
        
        struct BulletHitEvent : public IEvent {
            EntityId bulletId;
            EntityId targetId;
            Vector2 hitPosition;
            int damage;
            bool wasCritical;
            
            BulletHitEvent(EntityId bullet, EntityId target, const Vector2& pos, int dmg, bool crit) 
                : bulletId(bullet), targetId(target), hitPosition(pos), damage(dmg), wasCritical(crit) {}
        };
        
        struct EnemyDeathEvent : public IEvent {
            EntityId enemyId;
            Vector2 position;
            EnemyType enemyType;
            int experienceReward;
            int materialReward;
            EntityId killerId; // Who killed the enemy
            
            EnemyDeathEvent(EntityId enemy, const Vector2& pos, EnemyType type, int exp, int mat, EntityId killer) 
                : enemyId(enemy), position(pos), enemyType(type), experienceReward(exp), materialReward(mat), killerId(killer) {}
        };
        
        // =============================================================================
        // Wave and Spawning Events
        // =============================================================================
        
        struct WaveStartEvent : public IEvent {
            int waveNumber;
            float waveDuration;
            
            WaveStartEvent(int wave, float duration) : waveNumber(wave), waveDuration(duration) {}
        };
        
        struct WaveEndEvent : public IEvent {
            int waveNumber;
            bool successful;
            int enemiesKilled;
            int materialsEarned;
            
            WaveEndEvent(int wave, bool success, int killed, int materials) 
                : waveNumber(wave), successful(success), enemiesKilled(killed), materialsEarned(materials) {}
        };
        
        struct EnemySpawnEvent : public IEvent {
            EntityId enemyId;
            Vector2 spawnPosition;
            EnemyType enemyType;
            
            EnemySpawnEvent(EntityId id, const Vector2& pos, EnemyType type) 
                : enemyId(id), spawnPosition(pos), enemyType(type) {}
        };
        
        // =============================================================================
        // UI Events
        // =============================================================================
        
        struct ShowUIScreenEvent : public IEvent {
            std::string screenName;
            
            ShowUIScreenEvent(const std::string& name) : screenName(name) {}
        };
        
        struct HideUIScreenEvent : public IEvent {
            std::string screenName;
            
            HideUIScreenEvent(const std::string& name) : screenName(name) {}
        };
        
        struct ShopOpenEvent : public IEvent {
            int waveNumber;
            int playerMaterials;
            
            ShopOpenEvent(int wave, int materials) : waveNumber(wave), playerMaterials(materials) {}
        };
        
        struct ShopPurchaseEvent : public IEvent {
            EntityId playerId;
            std::string itemName;
            int cost;
            bool successful;
            
            ShopPurchaseEvent(EntityId player, const std::string& item, int price, bool success) 
                : playerId(player), itemName(item), cost(price), successful(success) {}
        };
        
        // =============================================================================
        // Audio Events
        // =============================================================================
        
        struct PlaySoundEvent : public IEvent {
            std::string soundName;
            Vector2 position;
            float volume;
            bool positional;
            
            PlaySoundEvent(const std::string& name, const Vector2& pos = Vector2(0, 0), float vol = 1.0f, bool isPositional = false) 
                : soundName(name), position(pos), volume(vol), positional(isPositional) {}
        };
        
        struct PlayMusicEvent : public IEvent {
            std::string musicName;
            bool loop;
            float volume;
            
            PlayMusicEvent(const std::string& name, bool shouldLoop = true, float vol = 1.0f) 
                : musicName(name), loop(shouldLoop), volume(vol) {}
        };
        
        // =============================================================================
        // Input Events
        // =============================================================================
        
        enum class InputAction {
            MOVE_UP,
            MOVE_DOWN,
            MOVE_LEFT,
            MOVE_RIGHT,
            FIRE,
            DASH,
            PAUSE,
            SCREENSHOT,
            SHOP_CONFIRM,
            SHOP_CANCEL
        };
        
        struct InputActionEvent : public IEvent {
            InputAction action;
            bool pressed;    // true for press, false for release
            float intensity; // For analog inputs (0.0 to 1.0)
            
            InputActionEvent(InputAction act, bool press, float intense = 1.0f) 
                : action(act), pressed(press), intensity(intense) {}
        };
        
        struct MouseMoveEvent : public IEvent {
            Vector2 position;
            Vector2 delta;
            
            MouseMoveEvent(const Vector2& pos, const Vector2& del) : position(pos), delta(del) {}
        };
        
    } // namespace Core
} // namespace BrotatoGame
