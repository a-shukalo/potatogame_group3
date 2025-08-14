#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <stdexcept>

namespace BrotatoGame {
    namespace Core {
        
        // Service Locator for dependency injection and service management
        class ServiceLocator {
        public:
            ServiceLocator() = default;
            ~ServiceLocator() = default;
            
            // Register a service instance
            template<typename ServiceType>
            void registerService(std::shared_ptr<ServiceType> service) {
                auto typeIndex = std::type_index(typeid(ServiceType));
                services[typeIndex] = service;
            }
            
            // Get a service instance
            template<typename ServiceType>
            std::shared_ptr<ServiceType> getService() {
                auto typeIndex = std::type_index(typeid(ServiceType));
                auto it = services.find(typeIndex);
                
                if (it != services.end()) {
                    return std::static_pointer_cast<ServiceType>(it->second);
                }
                
                return nullptr; // Service not found
            }
            
            // Get a service instance (throws if not found)
            template<typename ServiceType>
            std::shared_ptr<ServiceType> requireService() {
                auto service = getService<ServiceType>();
                if (!service) {
                    throw std::runtime_error("Required service not found: " + std::string(typeid(ServiceType).name()));
                }
                return service;
            }
            
            // Check if a service is registered
            template<typename ServiceType>
            bool hasService() {
                auto typeIndex = std::type_index(typeid(ServiceType));
                return services.find(typeIndex) != services.end();
            }
            
            // Unregister a service
            template<typename ServiceType>
            void unregisterService() {
                auto typeIndex = std::type_index(typeid(ServiceType));
                services.erase(typeIndex);
            }
            
            // Clear all services
            void clearServices() {
                services.clear();
            }
            
        private:
            std::unordered_map<std::type_index, std::shared_ptr<void>> services;
        };
        
        // Global service locator instance (for easy access)
        class Services {
        public:
            static ServiceLocator& getInstance() {
                static ServiceLocator instance;
                return instance;
            }
            
            template<typename ServiceType>
            static void Register(std::shared_ptr<ServiceType> service) {
                getInstance().registerService<ServiceType>(service);
            }
            
            template<typename ServiceType>
            static std::shared_ptr<ServiceType> Get() {
                return getInstance().getService<ServiceType>();
            }
            
            template<typename ServiceType>
            static std::shared_ptr<ServiceType> Require() {
                return getInstance().requireService<ServiceType>();
            }
            
        private:
            Services() = default;
        };
        
    } // namespace Core
} // namespace BrotatoGame
