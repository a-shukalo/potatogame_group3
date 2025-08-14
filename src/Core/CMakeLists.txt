# =============================================================================
# Core Module - CMakeLists.txt
# =============================================================================
# Handles: Engine foundation, Event system, Service locator, Math utilities

add_library(BrotatoCore STATIC
    # Engine
    Engine/Application.cpp
    Engine/Application.h
    Engine/ServiceLocator.h
    
    # Events  
    Events/EventManager.h
    Events/EventManager.cpp
    Events/GameEvents.h
    
    # Math
    Math/Vector2.h
    Math/Vector2.cpp
)

# Include directories
target_include_directories(BrotatoCore 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link SDL2 for the core engine
target_link_libraries(BrotatoCore 
    PUBLIC 
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    PRIVATE
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

# Compiler features
target_compile_features(BrotatoCore 
    PUBLIC 
        cxx_std_17
)

# Set target properties
set_target_properties(BrotatoCore PROPERTIES
    OUTPUT_NAME "BrotatoCore"
    DEBUG_POSTFIX "_d"
    POSITION_INDEPENDENT_CODE ON
)

# Export headers for other modules
target_include_directories(BrotatoCore
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
