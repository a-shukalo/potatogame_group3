#include "Core/Engine/Application.h"
#include "Core/Engine/ServiceLocator.h"
#include "Core/Events/EventManager.h"
#include "Rendering/Renderer/SDL2Renderer.h"
#include "Input/InputSystem/InputManager.h"
#include "Gameplay/World/GameplayManager.h"
#include <iostream>
#include <memory>

using namespace BrotatoGame;

int main(int argc, char* args[]) {
    std::cout << "=============================================================================\n";
    std::cout << "Brotato Game - Modular Architecture\n";
    std::cout << "=============================================================================\n";
    
    // Create the application
    Core::Application app;
    
    // Initialize the core application
    if (!app.initialize()) {
        std::cout << "Failed to initialize core application!" << std::endl;
        return -1;
    }
    
    std::cout << "Core application initialized successfully!\n";
    
    // Get the service locator for registering services  
    auto& services = Core::Services::getInstance();
    
    // Create and register the event manager
    auto eventManager = std::make_shared<Core::EventManager>();
    services.registerService<Core::EventManager>(eventManager);
    
    // Create and register the renderer
    auto renderer = std::make_shared<Rendering::SDL2Renderer>();
    services.registerService<Rendering::IRenderer>(renderer);
    services.registerService<Rendering::SDL2Renderer>(renderer);
    
    // Create and register the input manager
    auto inputManager = std::make_shared<Input::InputManager>();
    services.registerService<Input::InputManager>(inputManager);
    
    // Create and register the gameplay manager
    auto gameplayManager = std::make_shared<Gameplay::GameplayManager>();
    services.registerService<Gameplay::GameplayManager>(gameplayManager);
    
    std::cout << "All services registered successfully!\n";
    
    // EventManager doesn't need explicit initialization
    std::cout << "Event manager created!\n";
    
    if (!renderer->initialize()) {
        std::cout << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    std::cout << "Renderer initialized!\n";
    
    if (!inputManager->initialize()) {
        std::cout << "Failed to initialize input manager!" << std::endl;
        return -1;
    }
    std::cout << "Input manager initialized!\n";
    
    if (!gameplayManager->initialize()) {
        std::cout << "Failed to initialize gameplay manager!" << std::endl;
        return -1;
    }
    std::cout << "Gameplay manager initialized!\n";
    
    // Setup event subscriptions for demonstration
    if (eventManager) {
        // Subscribe to mouse move events
        eventManager->subscribe<Core::MouseMoveEvent>([](const Core::MouseMoveEvent& event) {
            // This will be handled by gameplay systems later
            // std::cout << "Mouse moved to: " << event.position.x << ", " << event.position.y << std::endl;
        });
        
        std::cout << "Event subscriptions set up!\n";
    }
    
    std::cout << "\n=== Starting Custom Game Loop ===\n";
    
    // Run our custom game loop instead of the core application loop
    // to have control over system updates
    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // Handle events
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }
        
        // Update systems manually
        inputManager->update(deltaTime);
        gameplayManager->update(deltaTime);
        
        // Render
        SDL_SetRenderDrawColor(app.getRenderer(), 120, 110, 100, 255);
        SDL_RenderClear(app.getRenderer());
        
        gameplayManager->render();
        
        SDL_RenderPresent(app.getRenderer());
        
        // Limit FPS
        SDL_Delay(16); // ~60 FPS
    }
    
    std::cout << "\n=== Game Loop Ended ===\n";
    
    // Cleanup is handled automatically by destructors and RAII
    std::cout << "Application shutdown complete.\n";
    
    return 0;
}
