# =============================================================================
# Input Module - CMakeLists.txt
# =============================================================================
# Handles: Input system, Audio system, Platform integration, Settings

add_library(BrotatoInput STATIC
    # Input System  
    InputSystem/InputManager.h
    InputSystem/InputManager.cpp
    
    # Audio System (to be added)
    # Audio/AudioManager.h
    # Audio/AudioManager.cpp
    
    # Platform (to be added)
    # Platform/MobileInput.h
)

# Include directories
target_include_directories(BrotatoInput
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(BrotatoInput
    PUBLIC
        BrotatoCore
    PRIVATE
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# Compiler features  
target_compile_features(BrotatoInput
    PUBLIC
        cxx_std_17
)

# Set target properties
set_target_properties(BrotatoInput PROPERTIES
    OUTPUT_NAME "BrotatoInput"
    DEBUG_POSTFIX "_d" 
    POSITION_INDEPENDENT_CODE ON
)

# Export headers for other modules
target_include_directories(BrotatoInput
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)
