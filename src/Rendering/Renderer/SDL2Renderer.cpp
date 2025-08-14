#include "SDL2Renderer.h"
#include "../../Core/Engine/ServiceLocator.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include <cmath>

namespace BrotatoGame {
    namespace Rendering {
        
        SDL2Renderer::SDL2Renderer() : renderer(nullptr), defaultFont(nullptr) {}
        
        SDL2Renderer::~SDL2Renderer() {
            shutdown();
        }
        
        bool SDL2Renderer::initialize() {
            // Get SDL renderer from the application
            auto app = Core::Application::getInstance();
            if (!app) {
                std::cout << "SDL2Renderer: Application instance not available!" << std::endl;
                return false;
            }
            
            // Get the SDL renderer from the application
            renderer = app->getRenderer();
            if (!renderer) {
                std::cout << "SDL2Renderer: Failed to get SDL_Renderer from Application!" << std::endl;
                return false;
            }
            
            // Initialize SDL_ttf
            if (TTF_Init() == -1) {
                std::cout << "SDL2Renderer: Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
                return false;
            }
            
            // Set default font path
            defaultFontPath = "assets/fonts/default.ttf";
            
            // Load default font
            std::cout << "SDL2Renderer: Attempting to load font from: " << defaultFontPath << std::endl;
            defaultFont = TTF_OpenFont(defaultFontPath.c_str(), 16);
            if (!defaultFont) {
                std::cout << "SDL2Renderer: Failed to load default font from " << defaultFontPath << ": " << TTF_GetError() << std::endl;
                // Don't fail initialization - we can still use bitmap text
            } else {
                std::cout << "SDL2Renderer: Successfully loaded default font!" << std::endl;
            }
            
            std::cout << "SDL2Renderer initialized successfully!" << std::endl;
            return true;
        }
        
        void SDL2Renderer::shutdown() {
            // Clean up texture cache
            for (auto& pair : textureCache) {
                if (pair.second) {
                    SDL_DestroyTexture(pair.second);
                }
            }
            textureCache.clear();
            
            // Clean up fonts
            unloadFonts();
            if (defaultFont) {
                TTF_CloseFont(defaultFont);
                defaultFont = nullptr;
            }
            
            // Quit SDL_ttf
            TTF_Quit();
            
            std::cout << "SDL2Renderer shut down." << std::endl;
        }
        
        void SDL2Renderer::beginFrame() {
            if (renderer) {
                SDL_SetRenderDrawColor(renderer, 120, 110, 100, 255);
                SDL_RenderClear(renderer);
            }
        }
        
        void SDL2Renderer::endFrame() {
            if (renderer) {
                SDL_RenderPresent(renderer);
            }
        }
        
        void SDL2Renderer::clear(const Color& color) {
            if (renderer) {
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderClear(renderer);
            }
        }
        
        void SDL2Renderer::drawRectangle(const Rectangle& rect, const Color& color, bool filled) {
            if (!renderer) return;
            
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            
            SDL_Rect sdlRect = {
                static_cast<int>(rect.x),
                static_cast<int>(rect.y),
                static_cast<int>(rect.width),
                static_cast<int>(rect.height)
            };
            
            if (filled) {
                SDL_RenderFillRect(renderer, &sdlRect);
            } else {
                SDL_RenderDrawRect(renderer, &sdlRect);
            }
        }
        
        void SDL2Renderer::drawCircle(const Vector2& center, float radius, const Color& color, bool filled) {
            if (!renderer) return;
            
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            
            int centerX = static_cast<int>(center.x);
            int centerY = static_cast<int>(center.y);
            int r = static_cast<int>(radius);
            
            if (filled) {
                // Draw filled circle using scan line algorithm
                for (int y = -r; y <= r; y++) {
                    int width = static_cast<int>(sqrt(r * r - y * y)) * 2;
                    SDL_RenderDrawLine(renderer, 
                                     centerX - width/2, centerY + y,
                                     centerX + width/2, centerY + y);
                }
            } else {
                // Draw circle outline using Bresenham's algorithm
                drawCirclePoints(centerX, centerY, 0, r, color);
                int x = 0, y = r;
                int decision = 3 - 2 * r;
                
                while (x <= y) {
                    drawCirclePoints(centerX, centerY, x, y, color);
                    x++;
                    if (decision > 0) {
                        y--;
                        decision += 4 * (x - y) + 10;
                    } else {
                        decision += 4 * x + 6;
                    }
                }
            }
        }
        
        void SDL2Renderer::drawLine(const Vector2& start, const Vector2& end, const Color& color, float thickness) {
            if (!renderer) return;
            
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            
            if (thickness <= 1.0f) {
                SDL_RenderDrawLine(renderer,
                                 static_cast<int>(start.x), static_cast<int>(start.y),
                                 static_cast<int>(end.x), static_cast<int>(end.y));
            } else {
                // Draw thick line as a series of parallel lines
                Vector2 direction = (end - start).normalized();
                Vector2 perpendicular(-direction.y, direction.x);
                
                int halfThickness = static_cast<int>(thickness / 2);
                for (int i = -halfThickness; i <= halfThickness; ++i) {
                    Vector2 offset = perpendicular * static_cast<float>(i);
                    Vector2 lineStart = start + offset;
                    Vector2 lineEnd = end + offset;
                    
                    SDL_RenderDrawLine(renderer,
                                     static_cast<int>(lineStart.x), static_cast<int>(lineStart.y),
                                     static_cast<int>(lineEnd.x), static_cast<int>(lineEnd.y));
                }
            }
        }
        
        TextureHandle SDL2Renderer::loadTexture(const std::string& filepath) {
            // Check cache first
            auto it = textureCache.find(filepath);
            if (it != textureCache.end()) {
                return it->second;
            }
            
            // Load new texture
            SDL_Texture* texture = loadSDLTexture(filepath);
            if (texture) {
                textureCache[filepath] = texture;
            }
            
            return texture;
        }
        
        void SDL2Renderer::unloadTexture(TextureHandle texture) {
            // Find and remove from cache
            for (auto it = textureCache.begin(); it != textureCache.end(); ++it) {
                if (it->second == texture) {
                    SDL_DestroyTexture(static_cast<SDL_Texture*>(texture));
                    textureCache.erase(it);
                    break;
                }
            }
        }
        
        void SDL2Renderer::drawTexture(TextureHandle texture, const Transform& transform) {
            if (!renderer || !texture) return;
            
            SDL_Texture* sdlTexture = static_cast<SDL_Texture*>(texture);
            
            // Get texture dimensions
            int textureWidth, textureHeight;
            SDL_QueryTexture(sdlTexture, nullptr, nullptr, &textureWidth, &textureHeight);
            
            // Calculate destination rectangle
            SDL_Rect destRect = {
                static_cast<int>(transform.position.x - textureWidth * transform.scale.x / 2),
                static_cast<int>(transform.position.y - textureHeight * transform.scale.y / 2),
                static_cast<int>(textureWidth * transform.scale.x),
                static_cast<int>(textureHeight * transform.scale.y)
            };
            
            // Convert rotation from radians to degrees
            double angle = transform.rotation * 180.0 / M_PI;
            
            SDL_RenderCopyEx(renderer, sdlTexture, nullptr, &destRect, angle, nullptr, SDL_FLIP_NONE);
        }
        
        void SDL2Renderer::drawTexture(TextureHandle texture, const Rectangle& sourceRect, const Rectangle& destRect) {
            if (!renderer || !texture) return;
            
            SDL_Texture* sdlTexture = static_cast<SDL_Texture*>(texture);
            
            SDL_Rect srcRect = {
                static_cast<int>(sourceRect.x),
                static_cast<int>(sourceRect.y),
                static_cast<int>(sourceRect.width),
                static_cast<int>(sourceRect.height)
            };
            
            SDL_Rect dstRect = {
                static_cast<int>(destRect.x),
                static_cast<int>(destRect.y),
                static_cast<int>(destRect.width),
                static_cast<int>(destRect.height)
            };
            
            SDL_RenderCopy(renderer, sdlTexture, &srcRect, &dstRect);
        }
        
        void SDL2Renderer::drawText(const std::string& text, const Vector2& position, const Color& color, int fontSize) {
            // Use TTF text by default for better quality
            drawTTFText(text, position, color, fontSize);
        }
        
        void SDL2Renderer::drawBitmapText(const std::string& text, const Vector2& position, const Color& color, int scale) {
            if (!renderer) return;
            
            setSDLColor(color);
            
            Vector2 currentPos = position;
            for (char c : text) {
                if (c >= '0' && c <= '9') {
                    renderBitmapDigit(c - '0', currentPos, color, scale);
                } else {
                    renderBitmapChar(c, currentPos, color, scale);
                }
                currentPos.x += 6 * scale;
            }
        }
        
        Vector2 SDL2Renderer::getTextSize(const std::string& text, int fontSize) {
            // Simple bitmap text sizing
            return Vector2(static_cast<float>(text.length() * 6), 7);
        }
        
        void SDL2Renderer::setBlendMode(bool enabled) {
            if (renderer) {
                SDL_SetRenderDrawBlendMode(renderer, enabled ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
            }
        }
        
        void SDL2Renderer::setViewport(const Rectangle& viewport) {
            if (renderer) {
                SDL_Rect viewportRect = {
                    static_cast<int>(viewport.x),
                    static_cast<int>(viewport.y),
                    static_cast<int>(viewport.width),
                    static_cast<int>(viewport.height)
                };
                SDL_RenderSetViewport(renderer, &viewportRect);
            }
        }
        
        // Private helper methods
        void SDL2Renderer::setSDLColor(const Color& color) {
            if (renderer) {
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            }
        }
        
        SDL_Texture* SDL2Renderer::loadSDLTexture(const std::string& filepath) {
            if (!renderer) return nullptr;
            
            SDL_Surface* surface = IMG_Load(filepath.c_str());
            if (!surface) {
                std::cout << "Failed to load image: " << filepath << " - " << IMG_GetError() << std::endl;
                return nullptr;
            }
            
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            
            if (!texture) {
                std::cout << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
                return nullptr;
            }
            
            return texture;
        }
        
        void SDL2Renderer::drawCirclePoints(int centerX, int centerY, int x, int y, const Color& color) {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        }
        
        void SDL2Renderer::renderBitmapDigit(int digit, const Vector2& position, const Color& color, int scale) {
            // Simple 5x7 bitmap font for digits (same as original implementation)
            static int digitPatterns[10][7] = {
                {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // 0
                {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110}, // 1
                {0b01110, 0b10001, 0b00001, 0b00110, 0b01000, 0b10000, 0b11111}, // 2
                {0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110}, // 3
                {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010}, // 4
                {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110}, // 5
                {0b01110, 0b10001, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110}, // 6
                {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000}, // 7
                {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110}, // 8
                {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b10001, 0b01110}  // 9
            };
            
            if (digit < 0 || digit > 9) return;
            
            for (int row = 0; row < 7; ++row) {
                for (int col = 0; col < 5; ++col) {
                    if (digitPatterns[digit][row] & (1 << (4 - col))) {
                        SDL_Rect pixel = {
                            static_cast<int>(position.x) + col * scale,
                            static_cast<int>(position.y) + row * scale,
                            scale,
                            scale
                        };
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                }
            }
        }
        
        void SDL2Renderer::renderBitmapChar(char c, const Vector2& position, const Color& color, int scale) {
            // Basic character rendering - would implement full character set
            if (c == ' ') return; // Space character, do nothing
            
            // For now, just draw a placeholder rectangle for unknown characters
            SDL_Rect charRect = {
                static_cast<int>(position.x),
                static_cast<int>(position.y),
                5 * scale,
                7 * scale
            };
            SDL_RenderDrawRect(renderer, &charRect);
        }
        
        void SDL2Renderer::drawTTFText(const std::string& text, const Vector2& position, const Color& color, int fontSize) {
            if (!renderer || text.empty()) return;
            
            TTF_Font* font = loadFont(fontSize);
            if (!font) {
                // Fallback to bitmap text if TTF loading fails
                drawBitmapText(text, position, color, 1);
                return;
            }
            
            // Create SDL color
            SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
            
            // Create text surface
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), sdlColor);
            if (!textSurface) {
                std::cout << "SDL2Renderer: Failed to create text surface: " << TTF_GetError() << std::endl;
                return;
            }
            
            // Create texture from surface
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (!textTexture) {
                std::cout << "SDL2Renderer: Failed to create text texture: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(textSurface);
                return;
            }
            
            // Render the text
            SDL_Rect destRect = {
                static_cast<int>(position.x),
                static_cast<int>(position.y),
                textSurface->w,
                textSurface->h
            };
            
            SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
            
            // Clean up
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        
        TTF_Font* SDL2Renderer::loadFont(int fontSize) {
            // Check if font is already cached
            auto it = fontCache.find(fontSize);
            if (it != fontCache.end() && it->second) {
                return it->second;
            }
            
            // Load font with specified size
            std::cout << "SDL2Renderer: Loading font " << defaultFontPath << " with size " << fontSize << std::endl;
            TTF_Font* font = TTF_OpenFont(defaultFontPath.c_str(), fontSize);
            if (font) {
                fontCache[fontSize] = font;
                std::cout << "SDL2Renderer: Successfully loaded font size " << fontSize << std::endl;
                return font;
            } else {
                std::cout << "SDL2Renderer: Failed to load font size " << fontSize << " from " << defaultFontPath << ": " << TTF_GetError() << std::endl;
                // Return default font if available
                if (defaultFont) {
                    std::cout << "SDL2Renderer: Using cached default font" << std::endl;
                } else {
                    std::cout << "SDL2Renderer: No default font available, will use bitmap fallback" << std::endl;
                }
                return defaultFont;
            }
        }
        
        void SDL2Renderer::unloadFonts() {
            for (auto& pair : fontCache) {
                if (pair.second) {
                    TTF_CloseFont(pair.second);
                }
            }
            fontCache.clear();
        }
        
    } // namespace Rendering
} // namespace BrotatoGame
