#include "WeaponFactory.h"
#include "../../Gameplay/Components/Components.h"
#include <iostream>

namespace BrotatoGame {
    namespace Content {
        
        using WeaponTier = Gameplay::WeaponTier;
        using Transform = Gameplay::Transform;
        using Weapon = Gameplay::Weapon;
        
        WeaponFactory::WeaponFactory() {
            initialize();
        }
        
        void WeaponFactory::initialize() {
            // Initialize weapon configurations for all types and tiers
            initializePistolConfigs();
            initializeSMGConfigs();
            initializeShotgunConfigs();
            initializeRailgunConfigs();
            initializeMeleeConfigs();
            
            // Initialize weapon combinations
            initializeCombinations();
            
            std::cout << "WeaponFactory initialized with all weapon configurations!" << std::endl;
        }
        
        void WeaponFactory::initializePistolConfigs() {
            // Pistol weapon configurations from original game
            std::unordered_map<WeaponTier, WeaponConfig> pistolConfigs;
            
            // Tier 1 Pistol
            WeaponConfig tier1;
            tier1.baseDamage = 20;
            tier1.fireRate = 1.2f;  // shots per second
            tier1.range = 400.0f;
            tier1.critChance = 0.05f;
            tier1.critMultiplier = 2.0f;
            tier1.pelletsPerShot = 1;
            tier1.spread = 0.0f;
            tier1.materialCost = 15;
            tier1.spritePath = "assets/weapons/pistol.png";
            pistolConfigs[WeaponTier::TIER_1] = tier1;
            
            // Tier 2 Pistol
            WeaponConfig tier2 = tier1;
            tier2.baseDamage = 30;
            tier2.fireRate = 1.4f;
            tier2.range = 450.0f;
            tier2.critChance = 0.08f;
            tier2.materialCost = 25;
            pistolConfigs[WeaponTier::TIER_2] = tier2;
            
            // Tier 3 Pistol
            WeaponConfig tier3 = tier2;
            tier3.baseDamage = 45;
            tier3.fireRate = 1.6f;
            tier3.range = 500.0f;
            tier3.critChance = 0.12f;
            tier3.materialCost = 40;
            pistolConfigs[WeaponTier::TIER_3] = tier3;
            
            // Tier 4 Pistol
            WeaponConfig tier4 = tier3;
            tier4.baseDamage = 65;
            tier4.fireRate = 1.8f;
            tier4.range = 550.0f;
            tier4.critChance = 0.15f;
            tier4.materialCost = 60;
            pistolConfigs[WeaponTier::TIER_4] = tier4;
            
            weaponConfigs[WeaponType::PISTOL] = pistolConfigs;
        }
        
        void WeaponFactory::initializeSMGConfigs() {
            // SMG weapon configurations - fast fire rate, lower damage
            std::unordered_map<WeaponTier, WeaponConfig> smgConfigs;
            
            // Tier 1 SMG
            WeaponConfig tier1;
            tier1.baseDamage = 12;
            tier1.fireRate = 4.0f;  // Very fast
            tier1.range = 300.0f;   // Shorter range
            tier1.critChance = 0.03f;
            tier1.critMultiplier = 1.8f;
            tier1.pelletsPerShot = 1;
            tier1.spread = 0.1f;    // Slight inaccuracy
            tier1.materialCost = 20;
            tier1.spritePath = "assets/weapons/smg.png";
            smgConfigs[WeaponTier::TIER_1] = tier1;
            
            // Tier 2 SMG
            WeaponConfig tier2 = tier1;
            tier2.baseDamage = 18;
            tier2.fireRate = 4.5f;
            tier2.range = 350.0f;
            tier2.critChance = 0.05f;
            tier2.materialCost = 30;
            smgConfigs[WeaponTier::TIER_2] = tier2;
            
            // Tier 3 SMG
            WeaponConfig tier3 = tier2;
            tier3.baseDamage = 26;
            tier3.fireRate = 5.0f;
            tier3.range = 400.0f;
            tier3.critChance = 0.08f;
            tier3.materialCost = 45;
            smgConfigs[WeaponTier::TIER_3] = tier3;
            
            // Tier 4 SMG
            WeaponConfig tier4 = tier3;
            tier4.baseDamage = 38;
            tier4.fireRate = 5.5f;
            tier4.range = 450.0f;
            tier4.critChance = 0.10f;
            tier4.materialCost = 65;
            smgConfigs[WeaponTier::TIER_4] = tier4;
            
            weaponConfigs[WeaponType::SMG] = smgConfigs;
        }
        
        void WeaponFactory::initializeShotgunConfigs() {
            // Shotgun weapon configurations - multiple pellets, close range
            std::unordered_map<WeaponTier, WeaponConfig> shotgunConfigs;
            
            // Tier 1 Shotgun
            WeaponConfig tier1;
            tier1.baseDamage = 15;  // Per pellet
            tier1.fireRate = 0.8f;  // Slower fire rate
            tier1.range = 250.0f;   // Short range
            tier1.critChance = 0.06f;
            tier1.critMultiplier = 2.2f;
            tier1.pelletsPerShot = 5;  // Multiple pellets
            tier1.spread = 0.3f;       // Wide spread
            tier1.materialCost = 25;
            tier1.spritePath = "assets/weapons/shotgun.png";
            shotgunConfigs[WeaponTier::TIER_1] = tier1;
            
            // Tier 2 Shotgun
            WeaponConfig tier2 = tier1;
            tier2.baseDamage = 22;
            tier2.fireRate = 0.9f;
            tier2.range = 300.0f;
            tier2.critChance = 0.08f;
            tier2.pelletsPerShot = 6;
            tier2.materialCost = 35;
            shotgunConfigs[WeaponTier::TIER_2] = tier2;
            
            // Tier 3 Shotgun
            WeaponConfig tier3 = tier2;
            tier3.baseDamage = 32;
            tier3.fireRate = 1.0f;
            tier3.range = 350.0f;
            tier3.critChance = 0.10f;
            tier3.pelletsPerShot = 7;
            tier3.materialCost = 50;
            shotgunConfigs[WeaponTier::TIER_3] = tier3;
            
            // Tier 4 Shotgun
            WeaponConfig tier4 = tier3;
            tier4.baseDamage = 45;
            tier4.fireRate = 1.1f;
            tier4.range = 400.0f;
            tier4.critChance = 0.12f;
            tier4.pelletsPerShot = 8;
            tier4.materialCost = 70;
            shotgunConfigs[WeaponTier::TIER_4] = tier4;
            
            weaponConfigs[WeaponType::SHOTGUN] = shotgunConfigs;
        }
        
        void WeaponFactory::initializeRailgunConfigs() {
            // Railgun weapon configurations - charge weapon, pierces all enemies
            std::unordered_map<WeaponTier, WeaponConfig> railgunConfigs;
            
            // Tier 1 Railgun
            WeaponConfig tier1;
            tier1.baseDamage = 80;  // High damage
            tier1.fireRate = 0.4f;  // Slow charge weapon
            tier1.range = 800.0f;   // Very long range
            tier1.critChance = 0.10f;
            tier1.critMultiplier = 2.5f;
            tier1.pelletsPerShot = 1;
            tier1.spread = 0.0f;    // Perfect accuracy
            tier1.isChargeWeapon = true;
            tier1.maxChargeTime = 2.0f;  // 2 second charge
            tier1.materialCost = 40;
            tier1.spritePath = "assets/weapons/sniper.png";
            railgunConfigs[WeaponTier::TIER_1] = tier1;
            
            // Tier 2 Railgun
            WeaponConfig tier2 = tier1;
            tier2.baseDamage = 120;
            tier2.fireRate = 0.5f;
            tier2.range = 900.0f;
            tier2.critChance = 0.15f;
            tier2.maxChargeTime = 1.8f;
            tier2.materialCost = 60;
            railgunConfigs[WeaponTier::TIER_2] = tier2;
            
            // Tier 3 Railgun
            WeaponConfig tier3 = tier2;
            tier3.baseDamage = 180;
            tier3.fireRate = 0.6f;
            tier3.range = 1000.0f;
            tier3.critChance = 0.20f;
            tier3.maxChargeTime = 1.6f;
            tier3.materialCost = 80;
            railgunConfigs[WeaponTier::TIER_3] = tier3;
            
            // Tier 4 Railgun
            WeaponConfig tier4 = tier3;
            tier4.baseDamage = 260;
            tier4.fireRate = 0.7f;
            tier4.range = 1200.0f;
            tier4.critChance = 0.25f;
            tier4.maxChargeTime = 1.4f;
            tier4.materialCost = 100;
            railgunConfigs[WeaponTier::TIER_4] = tier4;
            
            weaponConfigs[WeaponType::RAILGUN] = railgunConfigs;
        }
        
        void WeaponFactory::initializeMeleeConfigs() {
            // Melee weapon configurations - close range, high damage
            std::unordered_map<WeaponTier, WeaponConfig> meleeConfigs;
            
            // Tier 1 Melee
            WeaponConfig tier1;
            tier1.baseDamage = 35;
            tier1.fireRate = 1.5f;  // Attack speed
            tier1.range = 80.0f;    // Very short range (melee)
            tier1.critChance = 0.08f;
            tier1.critMultiplier = 2.2f;
            tier1.pelletsPerShot = 1;
            tier1.spread = 0.0f;
            tier1.materialCost = 10;  // Cheaper
            tier1.spritePath = "assets/weapons/brickonstick.png";
            meleeConfigs[WeaponTier::TIER_1] = tier1;
            
            // Tier 2 Melee
            WeaponConfig tier2 = tier1;
            tier2.baseDamage = 50;
            tier2.fireRate = 1.7f;
            tier2.range = 90.0f;
            tier2.critChance = 0.12f;
            tier2.materialCost = 15;
            meleeConfigs[WeaponTier::TIER_2] = tier2;
            
            // Tier 3 Melee
            WeaponConfig tier3 = tier2;
            tier3.baseDamage = 75;
            tier3.fireRate = 1.9f;
            tier3.range = 100.0f;
            tier3.critChance = 0.15f;
            tier3.materialCost = 25;
            meleeConfigs[WeaponTier::TIER_3] = tier3;
            
            // Tier 4 Melee
            WeaponConfig tier4 = tier3;
            tier4.baseDamage = 110;
            tier4.fireRate = 2.1f;
            tier4.range = 120.0f;
            tier4.critChance = 0.18f;
            tier4.materialCost = 35;
            meleeConfigs[WeaponTier::TIER_4] = tier4;
            
            weaponConfigs[WeaponType::MELEE_STICK] = meleeConfigs;
        }
        
        void WeaponFactory::initializeCombinations() {
            // Weapon combination table - like original game
            combineTable[{WeaponType::PISTOL, WeaponType::PISTOL}] = WeaponType::SMG;
            combineTable[{WeaponType::SMG, WeaponType::SMG}] = WeaponType::SHOTGUN;
            combineTable[{WeaponType::SHOTGUN, WeaponType::SHOTGUN}] = WeaponType::RAILGUN;
            combineTable[{WeaponType::PISTOL, WeaponType::MELEE_STICK}] = WeaponType::SHOTGUN;
        }
        
        EntityId WeaponFactory::createWeapon(WeaponType type, WeaponTier tier, EntityManager& entityManager, const Vector2& position) {
            auto entity = entityManager.createEntity();
            
            // Get weapon configuration
            WeaponConfig config = getWeaponConfig(type, tier);
            
            // Add Transform component
            entityManager.addComponent<Transform>(entity, Transform(position));
            
            // Add Weapon component
            Weapon weaponComponent;
            weaponComponent.type = type;
            weaponComponent.tier = tier;
            weaponComponent.damage = config.baseDamage;
            weaponComponent.fireRate = config.fireRate;
            weaponComponent.range = config.range;
            weaponComponent.critChance = config.critChance;
            weaponComponent.critMultiplier = config.critMultiplier;
            weaponComponent.pelletsPerShot = config.pelletsPerShot;
            weaponComponent.spread = config.spread;
            weaponComponent.isChargeWeapon = config.isChargeWeapon;
            weaponComponent.maxChargeTime = config.maxChargeTime;
            entityManager.addComponent<Weapon>(entity, weaponComponent);
            
            std::cout << "Created " << getWeaponName(type, tier) << " weapon entity: " << entity << std::endl;
            return entity;
        }
        
        WeaponConfig WeaponFactory::getWeaponConfig(WeaponType type, WeaponTier tier) const {
            auto typeIt = weaponConfigs.find(type);
            if (typeIt != weaponConfigs.end()) {
                auto tierIt = typeIt->second.find(tier);
                if (tierIt != typeIt->second.end()) {
                    return tierIt->second;
                }
            }
            
            // Return default config if not found
            WeaponConfig defaultConfig;
            defaultConfig.baseDamage = 10;
            defaultConfig.fireRate = 1.0f;
            defaultConfig.range = 400.0f;
            return defaultConfig;
        }
        
        bool WeaponFactory::canCombineWeapons(WeaponType type1, WeaponType type2) const {
            auto key = std::make_pair(type1, type2);
            return combineTable.find(key) != combineTable.end();
        }
        
        WeaponType WeaponFactory::getCombinedWeaponType(WeaponType type1, WeaponType type2) const {
            auto key = std::make_pair(type1, type2);
            auto it = combineTable.find(key);
            if (it != combineTable.end()) {
                return it->second;
            }
            return type1; // Default to first weapon type
        }
        
        EntityId WeaponFactory::combineWeapons(EntityId weapon1, EntityId weapon2, EntityManager& entityManager) {
            // Get weapon components
            auto weapon1Comp = entityManager.getComponent<Weapon>(weapon1);
            auto weapon2Comp = entityManager.getComponent<Weapon>(weapon2);
            
            if (!weapon1Comp || !weapon2Comp) {
                return weapon1; // Can't combine
            }
            
            // Check if combination is possible
            if (!canCombineWeapons(weapon1Comp->type, weapon2Comp->type)) {
                return weapon1; // Can't combine
            }
            
            // Get combined weapon type and higher tier
            WeaponType combinedType = getCombinedWeaponType(weapon1Comp->type, weapon2Comp->type);
            WeaponTier combinedTier = static_cast<WeaponTier>(std::max(
                static_cast<int>(weapon1Comp->tier),
                static_cast<int>(weapon2Comp->tier)
            ));
            
            // Create new combined weapon
            Vector2 position = entityManager.getComponent<Transform>(weapon1)->position;
            EntityId combinedWeapon = createWeapon(combinedType, combinedTier, entityManager, position);
            
            // Destroy original weapons
            entityManager.destroyEntity(weapon1);
            entityManager.destroyEntity(weapon2);
            
            std::cout << "Combined weapons into: " << getWeaponName(combinedType, combinedTier) << std::endl;
            return combinedWeapon;
        }
        
        int WeaponFactory::calculateMaterialCost(WeaponType type, WeaponTier tier) const {
            WeaponConfig config = getWeaponConfig(type, tier);
            return config.materialCost;
        }
        
        std::string WeaponFactory::getWeaponName(WeaponType type, WeaponTier tier) const {
            std::string tierName = "";
            switch (tier) {
                case WeaponTier::TIER_2: tierName = "Tier II "; break;
                case WeaponTier::TIER_3: tierName = "Tier III "; break;
                case WeaponTier::TIER_4: tierName = "Tier IV "; break;
                default: break; // Tier I has no prefix
            }
            
            std::string weaponName;
            switch (type) {
                case WeaponType::PISTOL: weaponName = "Pistol"; break;
                case WeaponType::SMG: weaponName = "SMG"; break;
                case WeaponType::SHOTGUN: weaponName = "Shotgun"; break;
                case WeaponType::RAILGUN: weaponName = "Railgun"; break;
                case WeaponType::MELEE_STICK: weaponName = "Brick on Stick"; break;
                default: weaponName = "Unknown"; break;
            }
            
            return tierName + weaponName;
        }
        
        std::string WeaponFactory::getWeaponDescription(WeaponType type, WeaponTier tier) const {
            switch (type) {
                case WeaponType::PISTOL:
                    return "Pierces 1 enemy, -50% damage to 2nd";
                case WeaponType::SMG:
                    return "Fast fire rate, inaccurate shots";
                case WeaponType::SHOTGUN:
                    return "Fires multiple pellets, close range";
                case WeaponType::RAILGUN:
                    return "Charge weapon, pierces all enemies";
                case WeaponType::MELEE_STICK:
                    return "Melee weapon, close combat";
                default:
                    return "Unknown weapon type";
            }
        }
        
    } // namespace Content
} // namespace BrotatoGame