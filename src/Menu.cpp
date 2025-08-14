#include "Menu.h"
#include <iostream>
#include <cstdio>

Menu::Menu() : active(false), canContinue(false), optionSelected(false), 
               currentOption(MenuOption::NEW_GAME),
               upKeyPressed(false), downKeyPressed(false), 
               enterKeyPressed(false), escKeyPressed(false), lastMousePressed(false),
               titleFont(nullptr), menuFont(nullptr) {
    
    // Initialize colors
    titleColor = {255, 255, 255, 255};    // White
    normalColor = {200, 200, 200, 255};   // Light gray
    selectedColor = {255, 255, 0, 255};   // Yellow
    disabledColor = {100, 100, 100, 255}; // Dark gray
    
    // Layout settings
    titleY = 150;
    menuStartY = 400;
    menuSpacing = 80;
}

Menu::~Menu() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (menuFont) {
        TTF_CloseFont(menuFont);
        menuFont = nullptr;
    }
}

void Menu::loadAssets(SDL_Renderer* renderer) {
    // Try to load fonts in order of preference
    const char* fontPaths[] = {
        "assets/fonts/default.ttf",
        "/System/Library/Fonts/Arial.ttf",  // macOS system font
        "/System/Library/Fonts/Helvetica.ttc",  // macOS system font
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/consola.ttf"
    };
    
    titleFont = nullptr;
    menuFont = nullptr;
    
    for (const char* fontPath : fontPaths) {
        if (!titleFont) {
            titleFont = TTF_OpenFont(fontPath, 48);
            if (titleFont) {
                std::cout << "Menu: Loaded title font from " << fontPath << std::endl;
            }
        }
        if (!menuFont) {
            menuFont = TTF_OpenFont(fontPath, 32);
            if (menuFont) {
                std::cout << "Menu: Loaded menu font from " << fontPath << std::endl;
            }
        }
        
        if (titleFont && menuFont) {
            break;
        }
    }
    
    if (!titleFont || !menuFont) {
        std::cout << "Menu: Using fallback text rendering (no TTF fonts available)" << std::endl;
    }
}

bool Menu::handleInput(const Uint8* keyState, bool escAvailable) {
    if (!active) return false;
    
    bool escProcessed = false;
    
    // Handle UP key
    if (keyState[SDL_SCANCODE_UP] && !upKeyPressed) {
        upKeyPressed = true;
        
        if (currentOption == MenuOption::NEW_GAME) {
            if (canContinue) {
                currentOption = MenuOption::CONTINUE;
            } else {
                currentOption = MenuOption::EXIT;
            }
        } else if (currentOption == MenuOption::EXIT) {
            currentOption = MenuOption::NEW_GAME;
        } else if (currentOption == MenuOption::CONTINUE) {
            currentOption = MenuOption::EXIT;
        }
    } else if (!keyState[SDL_SCANCODE_UP]) {
        upKeyPressed = false;
    }
    
    // Handle DOWN key
    if (keyState[SDL_SCANCODE_DOWN] && !downKeyPressed) {
        downKeyPressed = true;
        
        if (currentOption == MenuOption::CONTINUE) {
            currentOption = MenuOption::NEW_GAME;
        } else if (currentOption == MenuOption::NEW_GAME) {
            currentOption = MenuOption::EXIT;
        } else if (currentOption == MenuOption::EXIT) {
            if (canContinue) {
                currentOption = MenuOption::CONTINUE;
            } else {
                currentOption = MenuOption::NEW_GAME;
            }
        }
    } else if (!keyState[SDL_SCANCODE_DOWN]) {
        downKeyPressed = false;
    }
    
    // Handle ENTER key
    if (keyState[SDL_SCANCODE_RETURN] && !enterKeyPressed) {
        enterKeyPressed = true;
        activateCurrentOption();
    } else if (!keyState[SDL_SCANCODE_RETURN]) {
        enterKeyPressed = false;
    }
    
    // Handle ESCAPE key (acts as "Continue" if available, otherwise ignored)
    // ESC cooldown is managed globally by Game class
    if (keyState[SDL_SCANCODE_ESCAPE] && !escKeyPressed && escAvailable) {
        escKeyPressed = true;
        if (canContinue) {
            currentOption = MenuOption::CONTINUE;
            activateCurrentOption();
            escProcessed = true; // ESC was actually handled
        }
        // If canContinue is false, ESC is ignored (escProcessed remains false)
    } else if (!keyState[SDL_SCANCODE_ESCAPE]) {
        escKeyPressed = false;
    }
    
    return escProcessed;
}

void Menu::handleMouseInput(int mouseX, int mouseY, bool mousePressed) {
    if (!active) return;
    
    // Calculate menu item positions
    int windowWidth = 1920;  // From Game constants
    int centerX = windowWidth / 2;
    
    int continueY = menuStartY;
    int newGameY = canContinue ? menuStartY + menuSpacing : menuStartY;
    int exitY = canContinue ? menuStartY + 2 * menuSpacing : menuStartY + menuSpacing;
    
    // Check mouse hover over menu items
    int menuItemWidth = 300;
    int menuItemHeight = 50;
    
    MenuOption hoveredOption = currentOption;
    bool mouseOverButton = false;
    
    if (canContinue && mouseX >= centerX - menuItemWidth/2 && mouseX <= centerX + menuItemWidth/2 &&
        mouseY >= continueY - menuItemHeight/2 && mouseY <= continueY + menuItemHeight/2) {
        hoveredOption = MenuOption::CONTINUE;
        mouseOverButton = true;
    } else if (mouseX >= centerX - menuItemWidth/2 && mouseX <= centerX + menuItemWidth/2 &&
               mouseY >= newGameY - menuItemHeight/2 && mouseY <= newGameY + menuItemHeight/2) {
        hoveredOption = MenuOption::NEW_GAME;
        mouseOverButton = true;
    } else if (mouseX >= centerX - menuItemWidth/2 && mouseX <= centerX + menuItemWidth/2 &&
               mouseY >= exitY - menuItemHeight/2 && mouseY <= exitY + menuItemHeight/2) {
        hoveredOption = MenuOption::EXIT;
        mouseOverButton = true;
    }
    
    // Update current option only if mouse is over a button
    if (mouseOverButton) {
        currentOption = hoveredOption;
    }
    
    // Handle mouse clicks - only activate if mouse is over a button
    if (mousePressed && !lastMousePressed && mouseOverButton) {
        activateCurrentOption();
    }
    
    lastMousePressed = mousePressed;
}

void Menu::update(float deltaTime) {
    // Menu doesn't need complex updates, but we keep this for consistency
    (void)deltaTime; // Suppress unused parameter warning
}

void Menu::render(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
    if (!active) {
        return;
    }
    
    // Draw semi-transparent background overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect backgroundRect = {0, 0, windowWidth, windowHeight};
    SDL_RenderFillRect(renderer, &backgroundRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    int centerX = windowWidth / 2;
    
    // Render title
    if (titleFont) {
        renderText(renderer, "POTATO GAME", centerX - 150, titleY, titleColor, 48);
    } else {
        renderFallbackText(renderer, "POTATO GAME", centerX - 100, titleY, false);
    }
    
    // Render menu options with hover highlighting
    int yPos = menuStartY;
    int menuItemWidth = 300;
    int menuItemHeight = 50;
    
    if (canContinue) {
        bool selected = (currentOption == MenuOption::CONTINUE);
        SDL_Color color = selected ? selectedColor : normalColor;
        
        // Draw highlight background for selected item
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 100);
            SDL_Rect highlightRect = {centerX - menuItemWidth/2, yPos - menuItemHeight/2, menuItemWidth, menuItemHeight};
            SDL_RenderFillRect(renderer, &highlightRect);
            
            // Draw border around selected item
            SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, 255);
            SDL_RenderDrawRect(renderer, &highlightRect);
        }
        
        if (menuFont) {
            renderText(renderer, "CONTINUE", centerX - 60, yPos, color, 32);
        } else {
            renderFallbackText(renderer, "CONTINUE", centerX - 60, yPos, selected);
        }
        yPos += menuSpacing;
    }
    
    // New Game option
    {
        bool selected = (currentOption == MenuOption::NEW_GAME);
        SDL_Color color = selected ? selectedColor : normalColor;
        
        // Draw highlight background for selected item
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 100);
            SDL_Rect highlightRect = {centerX - menuItemWidth/2, yPos - menuItemHeight/2, menuItemWidth, menuItemHeight};
            SDL_RenderFillRect(renderer, &highlightRect);
            
            // Draw border around selected item
            SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, 255);
            SDL_RenderDrawRect(renderer, &highlightRect);
        }
        
        if (menuFont) {
            renderText(renderer, "NEW GAME", centerX - 70, yPos, color, 32);
        } else {
            renderFallbackText(renderer, "NEW GAME", centerX - 70, yPos, selected);
        }
        yPos += menuSpacing;
    }
    
    // Exit option
    {
        bool selected = (currentOption == MenuOption::EXIT);
        SDL_Color color = selected ? selectedColor : normalColor;
        
        // Draw highlight background for selected item
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 100);
            SDL_Rect highlightRect = {centerX - menuItemWidth/2, yPos - menuItemHeight/2, menuItemWidth, menuItemHeight};
            SDL_RenderFillRect(renderer, &highlightRect);
            
            // Draw border around selected item
            SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, 255);
            SDL_RenderDrawRect(renderer, &highlightRect);
        }
        
        if (menuFont) {
            renderText(renderer, "EXIT", centerX - 30, yPos, color, 32);
        } else {
            renderFallbackText(renderer, "EXIT", centerX - 30, yPos, selected);
        }
    }
    
    // Instructions at bottom
    if (menuFont) {
        renderText(renderer, "Use ARROW KEYS and ENTER to navigate", 
                  centerX - 200, windowHeight - 100, normalColor, 16);
        if (canContinue) {
            renderText(renderer, "Press ESC to continue game", 
                      centerX - 120, windowHeight - 60, normalColor, 16);
        }
    } else {
        renderFallbackText(renderer, "Use ARROW KEYS and ENTER", centerX - 120, windowHeight - 100, false);
        if (canContinue) {
            renderFallbackText(renderer, "ESC to continue", centerX - 80, windowHeight - 60, false);
        }
    }
}

void Menu::show(bool canContinue) {
    this->canContinue = canContinue;
    this->active = true;
    this->optionSelected = false;
    
    // Set default selected option based on context
    if (canContinue) {
        currentOption = MenuOption::CONTINUE;
    } else {
        currentOption = MenuOption::NEW_GAME;
    }
}

void Menu::hide() {
    active = false;
    optionSelected = false;
}

void Menu::selectOption(MenuOption option) {
    currentOption = option;
}

void Menu::activateCurrentOption() {
    optionSelected = true;
}

void Menu::renderText(SDL_Renderer* renderer, const char* text, int x, int y, 
                     SDL_Color color, int fontSize) {
    TTF_Font* font = (fontSize > 40) ? titleFont : menuFont;
    if (!font) {
        renderFallbackText(renderer, text, x, y, false);
        return;
    }
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    if (!textSurface) {
        renderFallbackText(renderer, text, x, y, false);
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        renderFallbackText(renderer, text, x, y, false);
        return;
    }
    
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    
    SDL_Rect destRect = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
    
    SDL_DestroyTexture(textTexture);
}

void Menu::renderFallbackText(SDL_Renderer* renderer, const char* text, int x, int y, bool selected) {
    // Enhanced fallback text rendering using rectangles with patterns
    SDL_Color color = selected ? selectedColor : normalColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    
    // Draw text with actual letter patterns for better visibility
    int charWidth = 20;
    int charHeight = 32;
    int spacing = 4;
    
    for (int i = 0; text[i] != '\0'; i++) {
        int charX = x + i * (charWidth + spacing);
        
        // Draw a distinctive pattern for each character
        char c = text[i];
        
        // Draw the character background/outline
        SDL_Rect bgRect = {charX, y, charWidth, charHeight};
        SDL_RenderDrawRect(renderer, &bgRect);
        
        // Draw letter-specific patterns
        if (c >= 'A' && c <= 'Z') {
            // Draw vertical lines for capitals
            SDL_Rect line1 = {charX + 2, y + 2, 4, charHeight - 4};
            SDL_Rect line2 = {charX + charWidth - 6, y + 2, 4, charHeight - 4};
            SDL_RenderFillRect(renderer, &line1);
            SDL_RenderFillRect(renderer, &line2);
            
            // Draw horizontal line for middle
            SDL_Rect hline = {charX + 2, y + charHeight/2, charWidth - 4, 3};
            SDL_RenderFillRect(renderer, &hline);
        } else if (c >= 'a' && c <= 'z') {
            // Draw smaller pattern for lowercase
            SDL_Rect smallRect = {charX + 4, y + 8, charWidth - 8, charHeight - 12};
            SDL_RenderFillRect(renderer, &smallRect);
        } else {
            // Draw simple filled rectangle for other characters
            SDL_Rect fillRect = {charX + 2, y + 2, charWidth - 4, charHeight - 4};
            SDL_RenderFillRect(renderer, &fillRect);
        }
        
        // Draw selection indicator if selected
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
            SDL_Rect selRect = {charX - 2, y - 2, charWidth + 4, charHeight + 4};
            SDL_RenderDrawRect(renderer, &selRect);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        }
    }
}
