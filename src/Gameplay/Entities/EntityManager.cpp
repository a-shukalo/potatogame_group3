#include "EntityManager.h"
#include "../../Core/Events/EventManager.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>

namespace BrotatoGame {
    namespace Gameplay {
        
        EntityManager::EntityManager() {
            // Pre-populate available entities queue
            for (EntityId id = 1; id < 10000; ++id) {  // Reserve plenty of IDs
                availableEntities.push(id);
            }
        }
        
        EntityId EntityManager::createEntity() {
            EntityId newEntity;
            
            if (!availableEntities.empty()) {
                newEntity = availableEntities.front();
                availableEntities.pop();
            } else {
                newEntity = nextEntityId++;
            }
            
            // Initialize empty signature for the entity
            entitySignatures[newEntity] = ComponentSignature{};
            
            // Publish entity creation event
            auto eventManager = Core::Services::Get<Core::EventManager>();
            if (eventManager) {
                eventManager->publish(Core::EntityCreatedEvent(newEntity, "Entity"));
            }
            
            return newEntity;
        }
        
        void EntityManager::destroyEntity(EntityId entity) {
            if (isEntityValid(entity)) {
                // Remove all components from this entity
                for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                    if (componentArrays[i]) {
                        componentArrays[i]->removeEntity(entity);
                    }
                }
                
                // Remove entity signature
                entitySignatures.erase(entity);
                
                // Return entity ID to available pool
                availableEntities.push(entity);
                
                // Publish entity destruction event
                auto eventManager = Core::Services::Get<Core::EventManager>();
                if (eventManager) {
                    eventManager->publish(Core::EntityDestroyedEvent(entity));
                }
            }
        }
        
        bool EntityManager::isEntityValid(EntityId entity) const {
            return entitySignatures.find(entity) != entitySignatures.end();
        }
        
    } // namespace Gameplay
} // namespace BrotatoGame
