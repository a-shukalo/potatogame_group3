#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Game;

enum class MenuOption {
    CONTINUE,
    NEW_GAME,
    EXIT
};

class Menu {
public:
    Menu();
    ~Menu();
    
    void loadAssets(SDL_Renderer* renderer);
    bool handleInput(const Uint8* keyState, bool escAvailable = true);
    void handleMouseInput(int mouseX, int mouseY, bool mousePressed);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight);
    
    // Menu state
    bool isActive() const { return active; }
    void setActive(bool active) { this->active = active; }
    void show(bool canContinue = false);
    void hide();
    
    // Option selection
    MenuOption getSelectedOption() const { return currentOption; }
    bool isOptionSelected() const { return optionSelected; }
    void resetSelection() { optionSelected = false; }
    
    // Menu visibility configuration
    void setCanContinue(bool canContinue) { this->canContinue = canContinue; }
    
private:
    void renderText(SDL_Renderer* renderer, const char* text, int x, int y, 
                   SDL_Color color, int fontSize = 24);
    void renderFallbackText(SDL_Renderer* renderer, const char* text, int x, int y, 
                           bool selected = false);
    void selectOption(MenuOption option);
    void activateCurrentOption();
    
    bool active;
    bool canContinue;
    bool optionSelected;
    MenuOption currentOption;
    
    // Input handling
    bool upKeyPressed;
    bool downKeyPressed;
    bool enterKeyPressed;
    bool escKeyPressed;
    bool lastMousePressed;
    
    // Note: ESC cooldown is managed by Game class globally
    
    // TTF Font
    TTF_Font* titleFont;
    TTF_Font* menuFont;
    
    // Colors
    SDL_Color titleColor;
    SDL_Color normalColor;
    SDL_Color selectedColor;
    SDL_Color disabledColor;
    
    // Layout
    int titleY;
    int menuStartY;
    int menuSpacing;
};
