#pragma once
#include <cmath>

namespace BrotatoGame {
    namespace Core {
        
        class Vector2 {
        public:
            float x, y;
            
            // Constructors
            Vector2() : x(0), y(0) {}
            Vector2(float x, float y) : x(x), y(y) {}
            
            // Basic arithmetic operations
            Vector2 operator+(const Vector2& other) const {
                return Vector2(x + other.x, y + other.y);
            }
            
            Vector2 operator-(const Vector2& other) const {
                return Vector2(x - other.x, y - other.y);
            }
            
            Vector2 operator*(float scalar) const {
                return Vector2(x * scalar, y * scalar);
            }
            
            Vector2 operator/(float scalar) const {
                return Vector2(x / scalar, y / scalar);
            }
            
            // Compound assignment operators
            Vector2& operator+=(const Vector2& other) {
                x += other.x;
                y += other.y;
                return *this;
            }
            
            Vector2& operator-=(const Vector2& other) {
                x -= other.x;
                y -= other.y;
                return *this;
            }
            
            Vector2& operator*=(float scalar) {
                x *= scalar;
                y *= scalar;
                return *this;
            }
            
            Vector2& operator/=(float scalar) {
                x /= scalar;
                y /= scalar;
                return *this;
            }
            
            // Comparison operators
            bool operator==(const Vector2& other) const {
                return (std::abs(x - other.x) < 0.001f && std::abs(y - other.y) < 0.001f);
            }
            
            bool operator!=(const Vector2& other) const {
                return !(*this == other);
            }
            
            // Vector operations
            float length() const {
                return std::sqrt(x * x + y * y);
            }
            
            float lengthSquared() const {
                return x * x + y * y;
            }
            
            Vector2 normalized() const {
                float len = length();
                if (len > 0.001f) {
                    return Vector2(x / len, y / len);
                }
                return Vector2(0, 0);
            }
            
            void normalize() {
                float len = length();
                if (len > 0.001f) {
                    x /= len;
                    y /= len;
                } else {
                    x = y = 0;
                }
            }
            
            float distance(const Vector2& other) const {
                return (*this - other).length();
            }
            
            float distanceSquared(const Vector2& other) const {
                return (*this - other).lengthSquared();
            }
            
            float dot(const Vector2& other) const {
                return x * other.x + y * other.y;
            }
            
            float cross(const Vector2& other) const {
                return x * other.y - y * other.x;
            }
            
            // Rotation
            Vector2 rotated(float angleRadians) const {
                float cos_a = std::cos(angleRadians);
                float sin_a = std::sin(angleRadians);
                return Vector2(x * cos_a - y * sin_a, x * sin_a + y * cos_a);
            }
            
            float angle() const {
                return std::atan2(y, x);
            }
            
            // Static utility functions
            static Vector2 zero() { return Vector2(0, 0); }
            static Vector2 one() { return Vector2(1, 1); }
            static Vector2 up() { return Vector2(0, -1); }
            static Vector2 down() { return Vector2(0, 1); }
            static Vector2 left() { return Vector2(-1, 0); }
            static Vector2 right() { return Vector2(1, 0); }
            
            static Vector2 fromAngle(float angleRadians) {
                return Vector2(std::cos(angleRadians), std::sin(angleRadians));
            }
            
            static Vector2 lerp(const Vector2& a, const Vector2& b, float t) {
                return a + (b - a) * t;
            }
        };
        
        // Allow scalar multiplication from left side
        inline Vector2 operator*(float scalar, const Vector2& vector) {
            return vector * scalar;
        }
        
    } // namespace Core
} // namespace BrotatoGame
