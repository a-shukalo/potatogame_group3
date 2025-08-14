#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>

namespace BrotatoGame {
    namespace Core {
        
        class ISystem;
        class EventManager;
        class ServiceLocator;
        
        class Application {
        public:
            Application();
            ~Application();
            
            bool initialize();
            void run();
            void shutdown();
            
            static Application* getInstance() { return instance; }
            SDL_Renderer* getRenderer() const { return renderer; }
            
        private:
            void update(float deltaTime);
            void render();
            void handleEvents();
            
            SDL_Window* window;
            SDL_Renderer* renderer;
            bool running;
            
            std::vector<std::unique_ptr<ISystem>> systems;
            std::unique_ptr<EventManager> eventManager;
            std::unique_ptr<ServiceLocator> serviceLocator;
            
            static Application* instance;
            
            static const int WINDOW_WIDTH = 1920;
            static const int WINDOW_HEIGHT = 1080;
        };
        
        // System interface that all game systems must implement
        class ISystem {
        public:
            virtual ~ISystem() = default;
            
            virtual bool initialize() = 0;
            virtual void update(float deltaTime) = 0;
            virtual void render() {}  // Optional for non-rendering systems
            virtual void shutdown() {}
            
            bool isActive() const { return active; }
            void setActive(bool state) { active = state; }
            
        protected:
            bool active = true;
        };
        
    } // namespace Core
} // namespace BrotatoGame
