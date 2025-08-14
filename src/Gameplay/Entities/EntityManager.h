#pragma once
#include "../Components/Components.h"
#include "../../Core/Events/GameEvents.h"
#include <unordered_map>
#include <queue>
#include <vector>
#include <memory>
#include <typeinfo>

namespace BrotatoGame {
    namespace Gameplay {
        
        // Forward declaration
        class IComponentArray;
        
        // Component array interface
        class IComponentArray {
        public:
            virtual ~IComponentArray() = default;
            virtual void removeEntity(EntityId entity) = 0;
        };
        
        // Templated component array
        template<typename T>
        class ComponentArray : public IComponentArray {
        public:
            void insertComponent(EntityId entity, const T& component) {
                entityToIndex[entity] = components.size();
                indexToEntity[components.size()] = entity;
                components.push_back(component);
            }
            
            void removeComponent(EntityId entity) {
                if (entityToIndex.find(entity) != entityToIndex.end()) {
                    size_t removedIndex = entityToIndex[entity];
                    size_t lastIndex = components.size() - 1;
                    
                    // Swap with last element
                    components[removedIndex] = components[lastIndex];
                    
                    // Update mappings
                    EntityId lastEntity = indexToEntity[lastIndex];
                    entityToIndex[lastEntity] = removedIndex;
                    indexToEntity[removedIndex] = lastEntity;
                    
                    // Remove mappings for deleted entity
                    entityToIndex.erase(entity);
                    indexToEntity.erase(lastIndex);
                    
                    components.pop_back();
                }
            }
            
            T* getComponent(EntityId entity) {
                auto it = entityToIndex.find(entity);
                if (it != entityToIndex.end()) {
                    return &components[it->second];
                }
                return nullptr;
            }
            
            void removeEntity(EntityId entity) override {
                removeComponent(entity);
            }
            
            std::vector<T>& getAllComponents() { return components; }
            const std::vector<T>& getAllComponents() const { return components; }
            
            std::vector<EntityId> getAllEntities() const {
                std::vector<EntityId> entities;
                for (const auto& pair : entityToIndex) {
                    entities.push_back(pair.first);
                }
                return entities;
            }
            
        private:
            std::vector<T> components;
            std::unordered_map<EntityId, size_t> entityToIndex;
            std::unordered_map<size_t, EntityId> indexToEntity;
        };
        
        // Entity Manager class
        class EntityManager {
        public:
            EntityManager();
            ~EntityManager() = default;
            
            // Entity management
            EntityId createEntity();
            void destroyEntity(EntityId entity);
            bool isEntityValid(EntityId entity) const;
            
            // Component management
            template<typename T>
            T* addComponent(EntityId entity) {
                ComponentTypeId typeId = getComponentTypeId<T>();
                
                if (!componentArrays[typeId]) {
                    componentArrays[typeId] = std::make_unique<ComponentArray<T>>();
                }
                
                auto componentArray = static_cast<ComponentArray<T>*>(componentArrays[typeId].get());
                componentArray->insertComponent(entity, T{});
                
                // Update entity signature
                entitySignatures[entity].set(typeId);
                
                return componentArray->getComponent(entity);
            }
            
            template<typename T>
            T* addComponent(EntityId entity, const T& component) {
                ComponentTypeId typeId = getComponentTypeId<T>();
                
                if (!componentArrays[typeId]) {
                    componentArrays[typeId] = std::make_unique<ComponentArray<T>>();
                }
                
                auto componentArray = static_cast<ComponentArray<T>*>(componentArrays[typeId].get());
                componentArray->insertComponent(entity, component);
                
                // Update entity signature
                entitySignatures[entity].set(typeId);
                
                return componentArray->getComponent(entity);
            }
            
            template<typename T>
            void removeComponent(EntityId entity) {
                ComponentTypeId typeId = getComponentTypeId<T>();
                
                if (componentArrays[typeId]) {
                    auto componentArray = static_cast<ComponentArray<T>*>(componentArrays[typeId].get());
                    componentArray->removeComponent(entity);
                    
                    // Update entity signature
                    entitySignatures[entity].reset(typeId);
                }
            }
            
            template<typename T>
            T* getComponent(EntityId entity) {
                ComponentTypeId typeId = getComponentTypeId<T>();
                
                if (componentArrays[typeId]) {
                    auto componentArray = static_cast<ComponentArray<T>*>(componentArrays[typeId].get());
                    return componentArray->getComponent(entity);
                }
                return nullptr;
            }
            
            template<typename T>
            bool hasComponent(EntityId entity) {
                ComponentTypeId typeId = getComponentTypeId<T>();
                return entitySignatures[entity].test(typeId);
            }
            
            // Get all entities with a specific component
            template<typename T>
            std::vector<EntityId> getEntitiesWithComponent() {
                ComponentTypeId typeId = getComponentTypeId<T>();
                
                if (componentArrays[typeId]) {
                    auto componentArray = static_cast<ComponentArray<T>*>(componentArrays[typeId].get());
                    return componentArray->getAllEntities();
                }
                return {};
            }
            
            // Get all entities with multiple components
            template<typename... Components>
            std::vector<EntityId> getEntitiesWith() {
                ComponentSignature signature;
                (signature.set(getComponentTypeId<Components>()), ...);
                
                std::vector<EntityId> entities;
                for (const auto& pair : entitySignatures) {
                    if ((pair.second & signature) == signature) {
                        entities.push_back(pair.first);
                    }
                }
                return entities;
            }
            
            // Get entity signature
            ComponentSignature getEntitySignature(EntityId entity) const {
                auto it = entitySignatures.find(entity);
                if (it != entitySignatures.end()) {
                    return it->second;
                }
                return ComponentSignature{};
            }
            
            // Get all components of a type
            template<typename T>
            std::vector<T>& getAllComponents() {
                ComponentTypeId typeId = getComponentTypeId<T>();
                
                if (!componentArrays[typeId]) {
                    componentArrays[typeId] = std::make_unique<ComponentArray<T>>();
                }
                
                auto componentArray = static_cast<ComponentArray<T>*>(componentArrays[typeId].get());
                return componentArray->getAllComponents();
            }
            
            // Get total entity count
            size_t getEntityCount() const { return entitySignatures.size() - availableEntities.size(); }
            
        private:
            std::unordered_map<EntityId, ComponentSignature> entitySignatures;
            std::array<std::unique_ptr<IComponentArray>, MAX_COMPONENTS> componentArrays;
            std::queue<EntityId> availableEntities;
            EntityId nextEntityId = 1; // Start from 1, 0 is reserved for INVALID_ENTITY_ID
        };
        
    } // namespace Gameplay
} // namespace BrotatoGame
