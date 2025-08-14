#include "Application.h"
#include "ServiceLocator.h"
#include "../Events/EventManager.h"
#include "../Events/GameEvents.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

namespace BrotatoGame {
    namespace Core {
        
        Application* Application::instance = nullptr;
        
        Application::Application() 
            : window(nullptr), renderer(nullptr), running(false) {
            if (instance == nullptr) {
                instance = this;
            }
        }
        
        Application::~Application() {
            shutdown();
            if (instance == this) {
                instance = nullptr;
            }
        }
        
        bool Application::initialize() {
            // Initialize SDL2
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
                std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
                return false;
            }
            
            // Initialize SDL_image
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
                std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
                return false;
            }
            
            // Initialize SDL_ttf
            if (TTF_Init() == -1) {
                std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
                return false;
            }
            
            // Create window
            window = SDL_CreateWindow("Brotato MVP", 
                                     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                     WINDOW_WIDTH, WINDOW_HEIGHT, 
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
            if (window == nullptr) {
                std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
                return false;
            }
            
            // Create renderer
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == nullptr) {
                std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
                return false;
            }
            
            // Create core services
            eventManager = std::make_unique<EventManager>();
            serviceLocator = std::make_unique<ServiceLocator>();
            
            // Register core services in the service locator
            Services::Register<EventManager>(std::shared_ptr<EventManager>(eventManager.get(), [](EventManager*){}));
            Services::Register<ServiceLocator>(std::shared_ptr<ServiceLocator>(serviceLocator.get(), [](ServiceLocator*){}));
            
            // Initialize all systems
            for (auto& system : systems) {
                if (!system->initialize()) {
                    std::cout << "Failed to initialize system!" << std::endl;
                    return false;
                }
            }
            
            running = true;
            std::cout << "Application initialized successfully!" << std::endl;
            return true;
        }
        
        void Application::run() {
            Uint32 lastTime = SDL_GetTicks();
            
            while (running) {
                Uint32 currentTime = SDL_GetTicks();
                float deltaTime = (currentTime - lastTime) / 1000.0f;
                lastTime = currentTime;
                
                // Cap delta time to prevent large jumps
                if (deltaTime > 0.05f) {
                    deltaTime = 0.05f;
                }
                
                handleEvents();
                update(deltaTime);
                render();
                
                // Cap frame rate to ~60 FPS
                SDL_Delay(16);
            }
        }
        
        void Application::shutdown() {
            // Shutdown all systems in reverse order
            for (auto it = systems.rbegin(); it != systems.rend(); ++it) {
                (*it)->shutdown();
            }
            systems.clear();
            
            // Clear services
            if (serviceLocator) {
                serviceLocator->clearServices();
            }
            
            // Clean up SDL
            if (renderer) {
                SDL_DestroyRenderer(renderer);
                renderer = nullptr;
            }
            
            if (window) {
                SDL_DestroyWindow(window);
                window = nullptr;
            }
            
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            
            running = false;
            std::cout << "Application shutdown complete." << std::endl;
        }
        
        void Application::update(float deltaTime) {
            // Update all active systems
            for (auto& system : systems) {
                if (system->isActive()) {
                    system->update(deltaTime);
                }
            }
            
            // Note: Individual systems should register themselves as systems
            // rather than being called manually here to avoid circular dependencies
            // Systems will be managed by the main application instead
        }
        
        void Application::render() {
            // Clear screen
            SDL_SetRenderDrawColor(renderer, 120, 110, 100, 255); // Light brown background
            SDL_RenderClear(renderer);
            
            // Render all active systems
            for (auto& system : systems) {
                if (system->isActive()) {
                    system->render();
                }
            }
            
            // Note: Rendering systems should register themselves as systems
            // to avoid circular dependencies
            
            // Present the frame
            SDL_RenderPresent(renderer);
        }
        
        void Application::handleEvents() {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    running = false;
                }
                
                // Handle ESC key for quit
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                
                // Publish mouse events 
                if (e.type == SDL_MOUSEMOTION && eventManager) {
                    Vector2 mousePos(static_cast<float>(e.motion.x), static_cast<float>(e.motion.y));
                    Vector2 delta(static_cast<float>(e.motion.xrel), static_cast<float>(e.motion.yrel));
                    eventManager->publish(MouseMoveEvent(mousePos, delta));
                }
                
                // Note: InputManager will handle SDL events through its own mechanism
                // This avoids circular dependency between Core and Input modules
            }
        }
        
    } // namespace Core
} // namespace BrotatoGame
