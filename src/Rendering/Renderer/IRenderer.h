#pragma once
#include "../../Core/Math/Vector2.h"
#include <string>

namespace BrotatoGame {
    namespace Rendering {
        
        using Vector2 = Core::Vector2;
        
        // Color structure
        struct Color {
            uint8_t r, g, b, a;
            
            Color() : r(255), g(255), b(255), a(255) {}
            Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) 
                : r(red), g(green), b(blue), a(alpha) {}
            
            // Predefined colors
            static Color white() { return Color(255, 255, 255); }
            static Color black() { return Color(0, 0, 0); }
            static Color red() { return Color(255, 0, 0); }
            static Color green() { return Color(0, 255, 0); }
            static Color blue() { return Color(0, 0, 255); }
            static Color yellow() { return Color(255, 255, 0); }
            static Color transparent() { return Color(0, 0, 0, 0); }
        };
        
        // Rectangle structure
        struct Rectangle {
            float x, y, width, height;
            
            Rectangle() : x(0), y(0), width(0), height(0) {}
            Rectangle(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}
            
            Vector2 getCenter() const { return Vector2(x + width/2, y + height/2); }
            bool contains(const Vector2& point) const {
                return point.x >= x && point.x <= x + width && 
                       point.y >= y && point.y <= y + height;
            }
        };
        
        // Transform structure
        struct Transform {
            Vector2 position;
            float rotation;  // In radians
            Vector2 scale;
            
            Transform() : position(0, 0), rotation(0), scale(1, 1) {}
            Transform(const Vector2& pos, float rot = 0, const Vector2& scl = Vector2(1, 1)) 
                : position(pos), rotation(rot), scale(scl) {}
        };
        
        // Texture handle (opaque)
        using TextureHandle = void*;
        
        // Renderer interface
        class IRenderer {
        public:
            virtual ~IRenderer() = default;
            
            // Initialization and cleanup
            virtual bool initialize() = 0;
            virtual void shutdown() = 0;
            
            // Frame management
            virtual void beginFrame() = 0;
            virtual void endFrame() = 0;
            virtual void clear(const Color& color) = 0;
            
            // Basic drawing primitives
            virtual void drawRectangle(const Rectangle& rect, const Color& color, bool filled = true) = 0;
            virtual void drawCircle(const Vector2& center, float radius, const Color& color, bool filled = true) = 0;
            virtual void drawLine(const Vector2& start, const Vector2& end, const Color& color, float thickness = 1.0f) = 0;
            
            // Texture management
            virtual TextureHandle loadTexture(const std::string& filepath) = 0;
            virtual void unloadTexture(TextureHandle texture) = 0;
            virtual void drawTexture(TextureHandle texture, const Transform& transform) = 0;
            virtual void drawTexture(TextureHandle texture, const Rectangle& sourceRect, const Rectangle& destRect) = 0;
            
            // Text rendering
            virtual void drawText(const std::string& text, const Vector2& position, const Color& color, int fontSize = 16) = 0;
            virtual void drawBitmapText(const std::string& text, const Vector2& position, const Color& color, int scale = 1) = 0;
            virtual void drawTTFText(const std::string& text, const Vector2& position, const Color& color, int fontSize = 16) = 0;
            
            // Utility functions
            virtual Vector2 getTextSize(const std::string& text, int fontSize = 16) = 0;
            virtual void setBlendMode(bool enabled) = 0;
            virtual void setViewport(const Rectangle& viewport) = 0;
        };
        
    } // namespace Rendering
} // namespace BrotatoGame
