#pragma once
#include "../../Core/Events/GameEvents.h"
#include "../../Gameplay/Entities/EntityManager.h"
#include "../../Gameplay/Components/Components.h"
#include <unordered_map>
#include <string>

namespace BrotatoGame {
    namespace Content {
        
        using EntityId = Core::EntityId;
        using WeaponType = Core::WeaponType;
        using EntityManager = Gameplay::EntityManager;
        using Vector2 = Core::Vector2;
        
        // Use WeaponTier from Components.h
        using WeaponTier = Gameplay::WeaponTier;
        
        // Weapon configuration structure
        struct WeaponConfig {
            int baseDamage;
            float fireRate;
            float range;
            float critChance;
            float critMultiplier;
            int pelletsPerShot;      // For shotgun
            float spread;            // Spread angle for shotgun
            bool isChargeWeapon;     // For railgun
            float maxChargeTime;     // For railgun
            int materialCost;
            std::string spritePath;
            std::string soundPath;
            
            WeaponConfig() 
                : baseDamage(10), fireRate(1.0f), range(400.0f), critChance(0.05f), critMultiplier(2.0f),
                  pelletsPerShot(1), spread(0.0f), isChargeWeapon(false), maxChargeTime(0.0f),
                  materialCost(10), spritePath(""), soundPath("") {}
        };
        
        // Weapon Factory class
        class WeaponFactory {
        public:
            WeaponFactory();
            ~WeaponFactory() = default;
            
            // Initialize weapon configurations
            void initialize();
            
            // Create a weapon entity
            EntityId createWeapon(WeaponType type, WeaponTier tier, EntityManager& entityManager, const Vector2& position = Vector2(0, 0));
            
            // Get weapon configuration
            WeaponConfig getWeaponConfig(WeaponType type, WeaponTier tier) const;
            
            // Weapon combination system
            bool canCombineWeapons(WeaponType type1, WeaponType type2) const;
            WeaponType getCombinedWeaponType(WeaponType type1, WeaponType type2) const;
            EntityId combineWeapons(EntityId weapon1, EntityId weapon2, EntityManager& entityManager);
            
            // Utility functions
            int calculateMaterialCost(WeaponType type, WeaponTier tier) const;
            std::string getWeaponName(WeaponType type, WeaponTier tier) const;
            std::string getWeaponDescription(WeaponType type, WeaponTier tier) const;
            
        private:
            // Hash function for pair keys
            struct PairHash {
                size_t operator()(const std::pair<WeaponType, WeaponType>& p) const {
                    return std::hash<int>()(static_cast<int>(p.first)) ^ 
                           (std::hash<int>()(static_cast<int>(p.second)) << 1);
                }
            };
            
            // Weapon configurations by type and tier
            std::unordered_map<WeaponType, std::unordered_map<WeaponTier, WeaponConfig>> weaponConfigs;
            
            // Weapon combination table
            std::unordered_map<std::pair<WeaponType, WeaponType>, WeaponType, PairHash> combineTable;
            
            // Initialize specific weapon types
            void initializePistolConfigs();
            void initializeSMGConfigs();
            void initializeShotgunConfigs();
            void initializeRailgunConfigs();
            void initializeMeleeConfigs();
            
            // Initialize weapon combinations
            void initializeCombinations();
            
            // Apply tier scaling to weapon config
            void applyTierScaling(WeaponConfig& config, WeaponTier tier) const;
        };
        
        // Weapon type utilities
        class WeaponUtils {
        public:
            static bool isRangedWeapon(WeaponType type) {
                return type != WeaponType::MELEE_STICK;
            }
            
            static bool isMeleeWeapon(WeaponType type) {
                return type == WeaponType::MELEE_STICK;
            }
            
            static bool isChargeWeapon(WeaponType type) {
                return type == WeaponType::RAILGUN;
            }
            
            static bool isSpreadWeapon(WeaponType type) {
                return type == WeaponType::SHOTGUN;
            }
            
            static std::string weaponTypeToString(WeaponType type) {
                switch (type) {
                    case WeaponType::PISTOL: return "Pistol";
                    case WeaponType::SMG: return "SMG";
                    case WeaponType::SHOTGUN: return "Shotgun";
                    case WeaponType::RAILGUN: return "Railgun";
                    case WeaponType::MELEE_STICK: return "Brick on Stick";
                    default: return "Unknown";
                }
            }
            
            static std::string weaponTierToString(WeaponTier tier) {
                switch (tier) {
                    case WeaponTier::TIER_1: return "Tier I";
                    case WeaponTier::TIER_2: return "Tier II";
                    case WeaponTier::TIER_3: return "Tier III";
                    case WeaponTier::TIER_4: return "Tier IV";
                    default: return "Unknown";
                }
            }
        };
        
    } // namespace Content
} // namespace BrotatoGame
