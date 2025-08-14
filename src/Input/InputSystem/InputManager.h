#pragma once
#include "../../Core/Engine/Application.h"
#include "../../Core/Events/GameEvents.h"
#include "../../Core/Math/Vector2.h"
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>

namespace BrotatoGame {
    namespace Input {
        
        using Vector2 = Core::Vector2;
        using InputAction = Core::InputAction;
        
        // Key code enumeration
        enum class KeyCode {
            // Letters
            A = SDLK_a, B = SDLK_b, C = SDLK_c, D = SDLK_d, E = SDLK_e, F = SDLK_f,
            G = SDLK_g, H = SDLK_h, I = SDLK_i, J = SDLK_j, K = SDLK_k, L = SDLK_l,
            M = SDLK_m, N = SDLK_n, O = SDLK_o, P = SDLK_p, Q = SDLK_q, R = SDLK_r,
            S = SDLK_s, T = SDLK_t, U = SDLK_u, V = SDLK_v, W = SDLK_w, X = SDLK_x,
            Y = SDLK_y, Z = SDLK_z,
            
            // Numbers
            NUM_0 = SDLK_0, NUM_1 = SDLK_1, NUM_2 = SDLK_2, NUM_3 = SDLK_3,
            NUM_4 = SDLK_4, NUM_5 = SDLK_5, NUM_6 = SDLK_6, NUM_7 = SDLK_7,
            NUM_8 = SDLK_8, NUM_9 = SDLK_9,
            
            // Arrow keys
            UP = SDLK_UP, DOWN = SDLK_DOWN, LEFT = SDLK_LEFT, RIGHT = SDLK_RIGHT,
            
            // Special keys
            SPACE = SDLK_SPACE,
            SHIFT = SDLK_LSHIFT,
            CTRL = SDLK_LCTRL,
            ALT = SDLK_LALT,
            ENTER = SDLK_RETURN,
            ESCAPE = SDLK_ESCAPE,
            TAB = SDLK_TAB,
            F12 = SDLK_F12,
            
            UNKNOWN = SDLK_UNKNOWN
        };
        
        // Mouse button enumeration
        enum class MouseButton {
            LEFT = SDL_BUTTON_LEFT,
            RIGHT = SDL_BUTTON_RIGHT,
            MIDDLE = SDL_BUTTON_MIDDLE
        };
        
        // Input Manager class
        class InputManager : public Core::ISystem {
        public:
            InputManager();
            ~InputManager() = default;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override;
            void shutdown() override;
            
            // Immediate input state
            bool isKeyDown(KeyCode key) const;
            bool isKeyPressed(KeyCode key) const;    // True only on the frame key was pressed
            bool isKeyReleased(KeyCode key) const;   // True only on the frame key was released
            
            bool isMouseButtonDown(MouseButton button) const;
            bool isMouseButtonPressed(MouseButton button) const;
            bool isMouseButtonReleased(MouseButton button) const;
            
            Vector2 getMousePosition() const;
            Vector2 getMouseDelta() const;
            
            // Action-based input system
            void setInputMapping(InputAction action, KeyCode key);
            void setInputMapping(InputAction action, MouseButton button);
            bool isActionActive(InputAction action) const;
            bool isActionPressed(InputAction action) const;
            bool isActionReleased(InputAction action) const;
            
            // Input handling (called by Application)
            void handleSDLEvent(const SDL_Event& event);
            
        private:
            // Key states
            std::unordered_map<KeyCode, bool> currentKeyState;
            std::unordered_map<KeyCode, bool> previousKeyState;
            
            // Mouse states
            std::unordered_map<MouseButton, bool> currentMouseState;
            std::unordered_map<MouseButton, bool> previousMouseState;
            Vector2 mousePosition;
            Vector2 mouseDelta;
            
            // Action mappings
            std::unordered_map<InputAction, KeyCode> actionToKeyMapping;
            std::unordered_map<InputAction, MouseButton> actionToMouseMapping;
            
            // Default input mappings
            void setupDefaultMappings();
            
            // Event publishing
            void publishInputEvents();
            
            // Helper functions
            KeyCode sdlKeyToKeyCode(SDL_Keycode sdlKey) const;
            MouseButton sdlButtonToMouseButton(Uint8 sdlButton) const;
        };
        
    } // namespace Input
} // namespace BrotatoGame
