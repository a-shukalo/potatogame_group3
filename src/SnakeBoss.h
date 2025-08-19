#pragma once
#include "Enemy.h"
#include "Bullet.h"
#include <vector>

struct SnakeSegment {
    Vector2 position;
    float radius;
    float timeSinceLastShot;
    float shootCooldown;
    BulletType bulletType;
    
    SnakeSegment(Vector2 pos, float r, BulletType type) 
        : position(pos), radius(r), timeSinceLastShot(0.0f), 
          shootCooldown(1.5f), bulletType(type) {}
};

class SnakeBoss : public Enemy {
public:
    SnakeBoss(Vector2 pos, SDL_Renderer* renderer, int currentWave);
    ~SnakeBoss() override;
    
    void update(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) override;
    void render(SDL_Renderer* renderer) override;
    void takeDamage(int damage) override;

    bool isBoss() const override { return true; }
    
private:
    // Snake characteristics
    int spawnWave;
    int maxHealth;
    std::vector<SnakeSegment> segments;
    float segmentDistance;     // Distance between segments
    float headRadius;
    
    // Movement tracking
    std::vector<Vector2> headPositionHistory;
    size_t maxHistoryLength;
    
    // Methods
    void initializeSegments();
    void updateMovement(float deltaTime, Vector2 playerPos);
    void updateSegmentPositions();
    void updateShooting(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets);
    void shootFromSegment(int segmentIndex, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets);
    void removeLastSegment();
    void renderHealthBar(SDL_Renderer* renderer);
    void renderSegment(SDL_Renderer* renderer, const SnakeSegment& segment, bool isHead = false);
};

// Factory function
std::unique_ptr<Enemy> CreateSnakeBoss(const Vector2& pos, SDL_Renderer* renderer, int wave);
