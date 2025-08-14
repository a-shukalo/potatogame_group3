# =============================================================================
# Content Module - CMakeLists.txt
# =============================================================================
# Handles: Weapons, Enemies, Progression, Game content and balance

add_library(BrotatoContent STATIC
    # Weapons
    Weapons/WeaponFactory.h
    Weapons/WeaponFactory.cpp
    
    # Enemies (to be added)
    # Enemies/EnemyFactory.h
    # Enemies/AIBehaviors.h
    
    # Progression (to be added)  
    # Progression/ProgressionSystem.h
    # Progression/LevelingSystem.h
    
    # Levels (to be added)
    # Levels/WaveManager.h
)

# Include directories
target_include_directories(BrotatoContent
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(BrotatoContent
    PUBLIC
        BrotatoCore
        BrotatoGameplay
    PRIVATE
        BrotatoRendering
)

# Compiler features
target_compile_features(BrotatoContent
    PUBLIC
        cxx_std_17
)

# Set target properties  
set_target_properties(BrotatoContent PROPERTIES
    OUTPUT_NAME "BrotatoContent"
    DEBUG_POSTFIX "_d"
    POSITION_INDEPENDENT_CODE ON
)

# Export headers for other modules
target_include_directories(BrotatoContent
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
