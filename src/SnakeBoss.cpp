#include "SnakeBoss.h"
#include "Bullet.h"
#include <cmath>
#include <iostream>

SnakeBoss::SnakeBoss(Vector2 pos, SDL_Renderer* renderer, int currentWave)
    : Enemy(pos, renderer), spawnWave(currentWave), segmentDistance(40.0f), 
      headRadius(50.0f), maxHistoryLength(50u) {
    
    // Set boss characteristics according to requirements
    health = 500;
    maxHealth = 500;
    radius = 30.0f;    // Default segment radius
    speed = 50.0f;
    damage = 30;
    
    // Initialize segments
    initializeSegments();
    
    std::cout << "Snake Boss spawned on wave " << currentWave << " with " << maxHealth << " health" << std::endl;
}

SnakeBoss::~SnakeBoss() {
    // Base destructor will handle sprite cleanup
}

void SnakeBoss::initializeSegments() {
    segments.clear();
    headPositionHistory.clear();
    
    // Create head segment
    segments.emplace_back(position, headRadius, BulletType::SNAKE_HEAD);
    segments[0].shootCooldown = 1.0f; // Head shoots faster
    
    // Create 4 body segments behind the head
    BulletType bodyTypes[] = {BulletType::SNAKE_BODY1, BulletType::SNAKE_BODY2, 
                              BulletType::SNAKE_BODY3, BulletType::SNAKE_BODY1};
    
    for (int i = 0; i < 4; i++) {
        Vector2 segmentPos = position - Vector2(segmentDistance * (i + 1), 0);
        segments.emplace_back(segmentPos, radius, bodyTypes[i]);
        segments[i + 1].shootCooldown = 1.5f + i * 0.2f; // Body segments shoot progressively slower
    }
    
    // Initialize position history with current head position
    for (size_t i = 0; i < maxHistoryLength; i++) {
        headPositionHistory.push_back(position);
    }
}

void SnakeBoss::updateMovement(float deltaTime, Vector2 playerPos) {
    // Head always moves toward player
    Vector2 direction = (playerPos - segments[0].position).normalized();
    Vector2 newHeadPos = segments[0].position + direction * speed * deltaTime;
    
    // Update head position
    segments[0].position = newHeadPos;
    
    // Store head position in history
    headPositionHistory.insert(headPositionHistory.begin(), newHeadPos);
    if (headPositionHistory.size() > maxHistoryLength) {
        headPositionHistory.pop_back();
    }
    
    // Update main position (for collision detection and other systems)
    position = segments[0].position;
    
    // Update body segments to follow the head
    updateSegmentPositions();
}

void SnakeBoss::updateSegmentPositions() {
    // Calculate how many history positions to skip per segment
    size_t historyStep = maxHistoryLength / segments.size();
    
    for (size_t i = 1; i < segments.size(); i++) {
        size_t historyIndex = i * historyStep;
        if (historyIndex < headPositionHistory.size()) {
            Vector2 targetPos = headPositionHistory[historyIndex];
            
            // Smooth interpolation to target position for more natural movement
            Vector2 diff = targetPos - segments[i].position;
            segments[i].position += diff * 0.1f;
        }
    }
}

void SnakeBoss::updateShooting(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    // Update shoot timers and shoot for each segment
    for (size_t i = 0; i < segments.size(); i++) {
        segments[i].timeSinceLastShot += deltaTime;
        
        if (segments[i].timeSinceLastShot >= segments[i].shootCooldown) {
            shootFromSegment(i, playerPos, bullets);
            segments[i].timeSinceLastShot = 0.0f;
        }
    }
}

void SnakeBoss::shootFromSegment(int segmentIndex, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    if (static_cast<size_t>(segmentIndex) >= segments.size()) return;
    
    const SnakeSegment& segment = segments[segmentIndex];
    Vector2 direction = (playerPos - segment.position).normalized();
    
    // Different bullet properties based on segment type
    int bulletDamage = damage;
    float bulletSpeed = 300.0f;
    float bulletRange = 600.0f;
    
    if (segmentIndex == 0) { // Head
        bulletDamage = damage + 10; // Head does more damage
        bulletSpeed = 350.0f;
        bulletRange = 700.0f;
    }
    
    bullets.push_back(std::make_unique<Bullet>(
        segment.position, direction,
        bulletDamage, bulletRange, bulletSpeed,
        segment.bulletType, /*enemyOwned*/ true
    ));
}

void SnakeBoss::removeLastSegment() {
    if (segments.size() > 1) { // Never remove the head
        segments.pop_back();
        std::cout << "Snake segment destroyed! Remaining segments: " << segments.size() << std::endl;
    }
}

void SnakeBoss::renderHealthBar(SDL_Renderer* renderer) {
    // Health bar dimensions
    int barWidth = 150;
    int barHeight = 15;
    int barX = static_cast<int>(segments[0].position.x - static_cast<float>(barWidth) / 2.0f);
    int barY = static_cast<int>(segments[0].position.y - headRadius - 25);
    
    // Background bar (Yellow border)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect bgRect = {barX - 2, barY - 2, barWidth + 4, barHeight + 4};
    SDL_RenderFillRect(renderer, &bgRect);
    
    // Black bar (missing health)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect fullRect = {barX, barY, barWidth, barHeight};
    SDL_RenderFillRect(renderer, &fullRect);
    
    // Red bar (current health)
    float healthPercent = static_cast<float>(health) / static_cast<float>(maxHealth);
    int healthWidth = static_cast<int>(barWidth * healthPercent);
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_Rect healthRect = {barX, barY, healthWidth, barHeight};
    SDL_RenderFillRect(renderer, &healthRect);
}

void SnakeBoss::renderSegment(SDL_Renderer* renderer, const SnakeSegment& segment, bool isHead) {
    // Set color based on segment type
    if (isHead) {
        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255); // Light red for head
    } else {
        // Different colors for body segments based on bullet type
        switch (segment.bulletType) {
            case BulletType::SNAKE_BODY1:
                SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255); // Light green
                break;
            case BulletType::SNAKE_BODY2:
                SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); // Light blue
                break;
            case BulletType::SNAKE_BODY3:
                SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255); // Light yellow
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // Gray fallback
                break;
        }
    }
    
    // Draw circle for segment
    int centerX = static_cast<int>(segment.position.x);
    int centerY = static_cast<int>(segment.position.y);
    int r = static_cast<int>(segment.radius);
    
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
    
    // Draw darker border for better visibility
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    for (int angle = 0; angle < 360; angle += 10) {
        float radAngle = angle * M_PI / 180.0f;
        int borderX = centerX + static_cast<int>(r * cos(radAngle));
        int borderY = centerY + static_cast<int>(r * sin(radAngle));
        SDL_RenderDrawPoint(renderer, borderX, borderY);
    }
}

void SnakeBoss::render(SDL_Renderer* renderer) {
    // Render all segments (body first, then head)
    for (size_t i = segments.size(); i > 0; i--) {
        bool isHead = (i - 1 == 0);
        renderSegment(renderer, segments[i - 1], isHead);
    }
    
    // Render health bar above head
    renderHealthBar(renderer);
}

void SnakeBoss::update(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    if (!alive) return;
    
    // Update movement
    updateMovement(deltaTime, playerPos);
    
    // Update shooting
    updateShooting(deltaTime, playerPos, bullets);
    
    // Base animation from parent class
    animationTimer += deltaTime;
    if (state == EnemyState::HIT) {
        hitTimer += deltaTime;
        if (hitTimer > 0.3f) {
            state = EnemyState::IDLE;
            hitTimer = 0.0f;
        }
    }
}

void SnakeBoss::takeDamage(int damage) {
    health -= damage;
    
    if (health <= 0) {
        health = 0;
        alive = false;
        std::cout << "Snake Boss defeated!" << std::endl;
    } else {
        // Remove last segment when taking damage
        removeLastSegment();
        std::cout << "Snake Boss health: " << health << "/" << maxHealth << std::endl;
    }
    
    hit(); // Show hit animation
}

// Factory function
std::unique_ptr<Enemy> CreateSnakeBoss(const Vector2& pos, SDL_Renderer* renderer, int wave) {
    return std::make_unique<SnakeBoss>(pos, renderer, wave);
}
