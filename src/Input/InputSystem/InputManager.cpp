#include "InputManager.h"
#include "../../Core/Engine/ServiceLocator.h"
#include "../../Core/Events/EventManager.h"
#include <iostream>

namespace BrotatoGame {
    namespace Input {
        
        InputManager::InputManager() {}
        
        bool InputManager::initialize() {
            setupDefaultMappings();
            std::cout << "InputManager initialized with default key mappings." << std::endl;
            return true;
        }
        
        void InputManager::update(float deltaTime) {
            // Update previous states
            previousKeyState = currentKeyState;
            previousMouseState = currentMouseState;
            
            // Get current keyboard state from SDL
            const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
            
            // Update key states for keys we care about
            currentKeyState[KeyCode::W] = keyboardState[SDL_SCANCODE_W];
            currentKeyState[KeyCode::A] = keyboardState[SDL_SCANCODE_A];
            currentKeyState[KeyCode::S] = keyboardState[SDL_SCANCODE_S];
            currentKeyState[KeyCode::D] = keyboardState[SDL_SCANCODE_D];
            currentKeyState[KeyCode::SHIFT] = keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];
            currentKeyState[KeyCode::SPACE] = keyboardState[SDL_SCANCODE_SPACE];
            currentKeyState[KeyCode::F12] = keyboardState[SDL_SCANCODE_F12];
            currentKeyState[KeyCode::ESCAPE] = keyboardState[SDL_SCANCODE_ESCAPE];
            
            // Get mouse state
            int mouseX, mouseY;
            Uint32 mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
            
            Vector2 newMousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
            mouseDelta = newMousePos - mousePosition;
            mousePosition = newMousePos;
            
            // Update mouse button states
            currentMouseState[MouseButton::LEFT] = (mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
            currentMouseState[MouseButton::RIGHT] = (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
            currentMouseState[MouseButton::MIDDLE] = (mouseButtons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
            
            // Publish input events based on state changes
            publishInputEvents();
        }
        
        void InputManager::shutdown() {
            currentKeyState.clear();
            previousKeyState.clear();
            currentMouseState.clear();
            previousMouseState.clear();
            actionToKeyMapping.clear();
            actionToMouseMapping.clear();
            
            std::cout << "InputManager shutdown complete." << std::endl;
        }
        
        bool InputManager::isKeyDown(KeyCode key) const {
            auto it = currentKeyState.find(key);
            return it != currentKeyState.end() && it->second;
        }
        
        bool InputManager::isKeyPressed(KeyCode key) const {
            auto current = currentKeyState.find(key);
            auto previous = previousKeyState.find(key);
            
            bool currentDown = (current != currentKeyState.end()) ? current->second : false;
            bool previousDown = (previous != previousKeyState.end()) ? previous->second : false;
            
            return currentDown && !previousDown;
        }
        
        bool InputManager::isKeyReleased(KeyCode key) const {
            auto current = currentKeyState.find(key);
            auto previous = previousKeyState.find(key);
            
            bool currentDown = (current != currentKeyState.end()) ? current->second : false;
            bool previousDown = (previous != previousKeyState.end()) ? previous->second : false;
            
            return !currentDown && previousDown;
        }
        
        bool InputManager::isMouseButtonDown(MouseButton button) const {
            auto it = currentMouseState.find(button);
            return it != currentMouseState.end() && it->second;
        }
        
        bool InputManager::isMouseButtonPressed(MouseButton button) const {
            auto current = currentMouseState.find(button);
            auto previous = previousMouseState.find(button);
            
            bool currentDown = (current != currentMouseState.end()) ? current->second : false;
            bool previousDown = (previous != previousMouseState.end()) ? previous->second : false;
            
            return currentDown && !previousDown;
        }
        
        bool InputManager::isMouseButtonReleased(MouseButton button) const {
            auto current = currentMouseState.find(button);
            auto previous = previousMouseState.find(button);
            
            bool currentDown = (current != currentMouseState.end()) ? current->second : false;
            bool previousDown = (previous != previousMouseState.end()) ? previous->second : false;
            
            return !currentDown && previousDown;
        }
        
        Vector2 InputManager::getMousePosition() const {
            return mousePosition;
        }
        
        Vector2 InputManager::getMouseDelta() const {
            return mouseDelta;
        }
        
        void InputManager::setInputMapping(InputAction action, KeyCode key) {
            actionToKeyMapping[action] = key;
        }
        
        void InputManager::setInputMapping(InputAction action, MouseButton button) {
            actionToMouseMapping[action] = button;
        }
        
        bool InputManager::isActionActive(InputAction action) const {
            // Check key mapping
            auto keyIt = actionToKeyMapping.find(action);
            if (keyIt != actionToKeyMapping.end()) {
                return isKeyDown(keyIt->second);
            }
            
            // Check mouse mapping
            auto mouseIt = actionToMouseMapping.find(action);
            if (mouseIt != actionToMouseMapping.end()) {
                return isMouseButtonDown(mouseIt->second);
            }
            
            return false;
        }
        
        bool InputManager::isActionPressed(InputAction action) const {
            // Check key mapping
            auto keyIt = actionToKeyMapping.find(action);
            if (keyIt != actionToKeyMapping.end()) {
                return isKeyPressed(keyIt->second);
            }
            
            // Check mouse mapping
            auto mouseIt = actionToMouseMapping.find(action);
            if (mouseIt != actionToMouseMapping.end()) {
                return isMouseButtonPressed(mouseIt->second);
            }
            
            return false;
        }
        
        bool InputManager::isActionReleased(InputAction action) const {
            // Check key mapping
            auto keyIt = actionToKeyMapping.find(action);
            if (keyIt != actionToKeyMapping.end()) {
                return isKeyReleased(keyIt->second);
            }
            
            // Check mouse mapping
            auto mouseIt = actionToMouseMapping.find(action);
            if (mouseIt != actionToMouseMapping.end()) {
                return isMouseButtonReleased(mouseIt->second);
            }
            
            return false;
        }
        
        void InputManager::handleSDLEvent(const SDL_Event& event) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    if (!event.key.repeat) { // Ignore key repeat
                        KeyCode key = sdlKeyToKeyCode(event.key.keysym.sym);
                        currentKeyState[key] = true;
                    }
                    break;
                    
                case SDL_KEYUP:
                    {
                        KeyCode key = sdlKeyToKeyCode(event.key.keysym.sym);
                        currentKeyState[key] = false;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    {
                        MouseButton button = sdlButtonToMouseButton(event.button.button);
                        currentMouseState[button] = true;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONUP:
                    {
                        MouseButton button = sdlButtonToMouseButton(event.button.button);
                        currentMouseState[button] = false;
                    }
                    break;
                    
                case SDL_MOUSEMOTION:
                    {
                        Vector2 newPosition(static_cast<float>(event.motion.x), 
                                          static_cast<float>(event.motion.y));
                        mouseDelta = newPosition - mousePosition;
                        mousePosition = newPosition;
                    }
                    break;
            }
        }
        
        void InputManager::setupDefaultMappings() {
            // Movement
            setInputMapping(InputAction::MOVE_UP, KeyCode::W);
            setInputMapping(InputAction::MOVE_DOWN, KeyCode::S);
            setInputMapping(InputAction::MOVE_LEFT, KeyCode::A);
            setInputMapping(InputAction::MOVE_RIGHT, KeyCode::D);
            
            // Combat
            setInputMapping(InputAction::FIRE, MouseButton::LEFT);
            setInputMapping(InputAction::DASH, KeyCode::SHIFT);
            
            // UI
            setInputMapping(InputAction::PAUSE, KeyCode::ESCAPE);
            setInputMapping(InputAction::SCREENSHOT, KeyCode::F12);
            setInputMapping(InputAction::SHOP_CONFIRM, KeyCode::ENTER);
            setInputMapping(InputAction::SHOP_CANCEL, KeyCode::ESCAPE);
        }
        
        void InputManager::publishInputEvents() {
            auto eventManager = Core::Services::Get<Core::EventManager>();
            if (!eventManager) return;
            
            // Publish action events
            for (const auto& mapping : actionToKeyMapping) {
                if (isActionPressed(mapping.first)) {
                    eventManager->publish(Core::InputActionEvent(mapping.first, true));
                } else if (isActionReleased(mapping.first)) {
                    eventManager->publish(Core::InputActionEvent(mapping.first, false));
                }
            }
            
            for (const auto& mapping : actionToMouseMapping) {
                if (isActionPressed(mapping.first)) {
                    eventManager->publish(Core::InputActionEvent(mapping.first, true));
                } else if (isActionReleased(mapping.first)) {
                    eventManager->publish(Core::InputActionEvent(mapping.first, false));
                }
            }
            
            // Publish mouse move event if there's any delta
            if (mouseDelta.length() > 0.01f) {
                eventManager->publish(Core::MouseMoveEvent(mousePosition, mouseDelta));
            }
        }
        
        KeyCode InputManager::sdlKeyToKeyCode(SDL_Keycode sdlKey) const {
            // Convert SDL key codes to our key codes
            switch (sdlKey) {
                case SDLK_a: return KeyCode::A;
                case SDLK_b: return KeyCode::B;
                case SDLK_c: return KeyCode::C;
                case SDLK_d: return KeyCode::D;
                case SDLK_e: return KeyCode::E;
                case SDLK_f: return KeyCode::F;
                case SDLK_g: return KeyCode::G;
                case SDLK_h: return KeyCode::H;
                case SDLK_i: return KeyCode::I;
                case SDLK_j: return KeyCode::J;
                case SDLK_k: return KeyCode::K;
                case SDLK_l: return KeyCode::L;
                case SDLK_m: return KeyCode::M;
                case SDLK_n: return KeyCode::N;
                case SDLK_o: return KeyCode::O;
                case SDLK_p: return KeyCode::P;
                case SDLK_q: return KeyCode::Q;
                case SDLK_r: return KeyCode::R;
                case SDLK_s: return KeyCode::S;
                case SDLK_t: return KeyCode::T;
                case SDLK_u: return KeyCode::U;
                case SDLK_v: return KeyCode::V;
                case SDLK_w: return KeyCode::W;
                case SDLK_x: return KeyCode::X;
                case SDLK_y: return KeyCode::Y;
                case SDLK_z: return KeyCode::Z;
                
                case SDLK_0: return KeyCode::NUM_0;
                case SDLK_1: return KeyCode::NUM_1;
                case SDLK_2: return KeyCode::NUM_2;
                case SDLK_3: return KeyCode::NUM_3;
                case SDLK_4: return KeyCode::NUM_4;
                case SDLK_5: return KeyCode::NUM_5;
                case SDLK_6: return KeyCode::NUM_6;
                case SDLK_7: return KeyCode::NUM_7;
                case SDLK_8: return KeyCode::NUM_8;
                case SDLK_9: return KeyCode::NUM_9;
                
                case SDLK_UP: return KeyCode::UP;
                case SDLK_DOWN: return KeyCode::DOWN;
                case SDLK_LEFT: return KeyCode::LEFT;
                case SDLK_RIGHT: return KeyCode::RIGHT;
                
                case SDLK_SPACE: return KeyCode::SPACE;
                case SDLK_LSHIFT: return KeyCode::SHIFT;
                case SDLK_RSHIFT: return KeyCode::SHIFT;
                case SDLK_LCTRL: return KeyCode::CTRL;
                case SDLK_RCTRL: return KeyCode::CTRL;
                case SDLK_LALT: return KeyCode::ALT;
                case SDLK_RALT: return KeyCode::ALT;
                case SDLK_RETURN: return KeyCode::ENTER;
                case SDLK_ESCAPE: return KeyCode::ESCAPE;
                case SDLK_TAB: return KeyCode::TAB;
                case SDLK_F12: return KeyCode::F12;
                
                default: return KeyCode::UNKNOWN;
            }
        }
        
        MouseButton InputManager::sdlButtonToMouseButton(Uint8 sdlButton) const {
            switch (sdlButton) {
                case SDL_BUTTON_LEFT: return MouseButton::LEFT;
                case SDL_BUTTON_RIGHT: return MouseButton::RIGHT;
                case SDL_BUTTON_MIDDLE: return MouseButton::MIDDLE;
                default: return MouseButton::LEFT; // Default fallback
            }
        }
        
    } // namespace Input
} // namespace BrotatoGame
