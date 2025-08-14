#pragma once
#include "IRenderer.h"
#include "../../Core/Engine/Application.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <memory>

namespace BrotatoGame {
    namespace Rendering {
        
        class SDL2Renderer : public IRenderer, public Core::ISystem {
        public:
            SDL2Renderer();
            ~SDL2Renderer() override;
            
            // ISystem interface
            bool initialize() override;
            void update(float deltaTime) override {}  // Renderer doesn't need update
            void render() override {}                 // Rendering handled by other systems
            void shutdown() override;
            
            // IRenderer interface
            void beginFrame() override;
            void endFrame() override;
            void clear(const Color& color) override;
            
            // Basic drawing primitives
            void drawRectangle(const Rectangle& rect, const Color& color, bool filled = true) override;
            void drawCircle(const Vector2& center, float radius, const Color& color, bool filled = true) override;
            void drawLine(const Vector2& start, const Vector2& end, const Color& color, float thickness = 1.0f) override;
            
            // Texture management
            TextureHandle loadTexture(const std::string& filepath) override;
            void unloadTexture(TextureHandle texture) override;
            void drawTexture(TextureHandle texture, const Transform& transform) override;
            void drawTexture(TextureHandle texture, const Rectangle& sourceRect, const Rectangle& destRect) override;
            
            // Text rendering
            void drawText(const std::string& text, const Vector2& position, const Color& color, int fontSize = 16) override;
            void drawBitmapText(const std::string& text, const Vector2& position, const Color& color, int scale = 1) override;
            void drawTTFText(const std::string& text, const Vector2& position, const Color& color, int fontSize = 16) override;
            
            // Utility functions
            Vector2 getTextSize(const std::string& text, int fontSize = 16) override;
            void setBlendMode(bool enabled) override;
            void setViewport(const Rectangle& viewport) override;
            
            // SDL2-specific functions
            SDL_Renderer* getSDLRenderer() const { return renderer; }
            
        private:
            SDL_Renderer* renderer;
            TTF_Font* defaultFont;
            
            // Font cache for different sizes
            std::unordered_map<int, TTF_Font*> fontCache;
            std::string defaultFontPath;
            
            // Texture cache
            std::unordered_map<std::string, SDL_Texture*> textureCache;
            
            // Bitmap font rendering
            void renderBitmapDigit(int digit, const Vector2& position, const Color& color, int scale);
            void renderBitmapChar(char c, const Vector2& position, const Color& color, int scale);
            
            // Helper functions
            void setSDLColor(const Color& color);
            SDL_Texture* loadSDLTexture(const std::string& filepath);
            void drawCirclePoints(int centerX, int centerY, int x, int y, const Color& color);
            
            // TTF font functions
            TTF_Font* loadFont(int fontSize);
            void unloadFonts();
        };
        
    } // namespace Rendering
} // namespace BrotatoGame
