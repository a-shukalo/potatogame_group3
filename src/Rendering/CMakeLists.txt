# =============================================================================
# Rendering Module - CMakeLists.txt  
# =============================================================================
# Handles: Graphics pipeline, UI system, Visual effects, Asset management

add_library(BrotatoRendering STATIC
    # Renderer
    Renderer/IRenderer.h
    Renderer/SDL2Renderer.h
    Renderer/SDL2Renderer.cpp
)

# Include directories
target_include_directories(BrotatoRendering
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(BrotatoRendering
    PUBLIC
        BrotatoCore
    PRIVATE
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

# Compiler features
target_compile_features(BrotatoRendering
    PUBLIC
        cxx_std_17
)

# Set target properties
set_target_properties(BrotatoRendering PROPERTIES
    OUTPUT_NAME "BrotatoRendering"
    DEBUG_POSTFIX "_d"
    POSITION_INDEPENDENT_CODE ON
)

# Export headers for other modules
target_include_directories(BrotatoRendering
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
