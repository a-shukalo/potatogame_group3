# =============================================================================
# Gameplay Module - CMakeLists.txt
# =============================================================================  
# Handles: ECS system, Game logic, Entity management, Game systems

add_library(BrotatoGameplay STATIC
    # Components
    Components/Components.h
    
    # Entities
    Entities/EntityManager.h
    Entities/EntityManager.cpp
    
    # Systems
    Systems/RenderSystem.h
    Systems/RenderSystem.cpp
    Systems/MovementSystem.h
    Systems/MovementSystem.cpp
    Systems/EnemySpawnSystem.h
    Systems/EnemySpawnSystem.cpp
    Systems/EnemyAISystem.h
    Systems/EnemyAISystem.cpp
    Systems/ProjectileSystem.h
    Systems/ProjectileSystem.cpp
    Systems/ShootingSystem.h
    Systems/ShootingSystem.cpp
    Systems/CollisionSystem.h
    Systems/CollisionSystem.cpp
    Systems/WaveSystem.h
    Systems/WaveSystem.cpp
    Systems/ShopSystem.h
    Systems/ShopSystem.cpp
    Systems/PickupSystem.h
    Systems/PickupSystem.cpp
    Systems/UISystem.h
    Systems/UISystem.cpp
    
    # World
    World/GameplayManager.h
    World/GameplayManager.cpp
)

# Include directories
target_include_directories(BrotatoGameplay
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies  
target_link_libraries(BrotatoGameplay
    PUBLIC
        BrotatoCore
        BrotatoRendering
)

# Compiler features
target_compile_features(BrotatoGameplay
    PUBLIC
        cxx_std_17
)

# Set target properties
set_target_properties(BrotatoGameplay PROPERTIES
    OUTPUT_NAME "BrotatoGameplay"  
    DEBUG_POSTFIX "_d"
    POSITION_INDEPENDENT_CODE ON
)

# Export headers for other modules
target_include_directories(BrotatoGameplay
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
