#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <typeinfo>

namespace BrotatoGame {
    namespace Core {
        
        // Base class for all events
        class IEvent {
        public:
            virtual ~IEvent() = default;
        };
        
        // Event Manager for decoupled communication between modules
        class EventManager {
        public:
            EventManager() = default;
            ~EventManager() = default;
            
            // Subscribe to an event type with a handler function
            template<typename EventType>
            void subscribe(std::function<void(const EventType&)> handler) {
                static_assert(std::is_base_of_v<IEvent, EventType>, "EventType must inherit from IEvent");
                
                auto typeIndex = std::type_index(typeid(EventType));
                auto wrapper = [handler](const void* event) {
                    handler(*static_cast<const EventType*>(event));
                };
                
                handlers[typeIndex].push_back(wrapper);
            }
            
            // Publish an event to all subscribers
            template<typename EventType>
            void publish(const EventType& event) {
                static_assert(std::is_base_of_v<IEvent, EventType>, "EventType must inherit from IEvent");
                
                auto typeIndex = std::type_index(typeid(EventType));
                auto it = handlers.find(typeIndex);
                
                if (it != handlers.end()) {
                    for (auto& handler : it->second) {
                        handler(&event);
                    }
                }
            }
            
            // Clear all handlers for a specific event type
            template<typename EventType>
            void clearHandlers() {
                auto typeIndex = std::type_index(typeid(EventType));
                handlers.erase(typeIndex);
            }
            
            // Clear all handlers
            void clearAllHandlers() {
                handlers.clear();
            }
            
        private:
            std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> handlers;
        };
        
    } // namespace Core
} // namespace BrotatoGame
