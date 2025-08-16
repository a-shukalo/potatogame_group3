#pragma once
#include "Enemy.h"

enum class BossAttackType {
    SINGLE_SHOT,
    SPREAD_SHOT,
    CIRCULAR_SHOT
};

enum class BossState {
    HUNTING,     // Chases the player
    CIRCLING,    // Circles around the player
    ATTACKING,   // Executes attacks
    RETREATING   // Retreats for next attack
};

class BossEnemy : public Enemy {
public:
    BossEnemy(Vector2 pos, SDL_Renderer* renderer, int currentWave);
    ~BossEnemy() override;
    
    void update(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) override;
    void render(SDL_Renderer* renderer) override;
    void takeDamage(int damage) override;

    bool isBoss() const override { return true; }
    
private:
    // Boss characteristics
    int spawnWave;
    int maxHealth;
    
    // State system
    BossState currentState;
    float stateTimer;
    float stateDuration;
    Vector2 targetPosition;
    
    // Attack system
    BossAttackType currentAttackType;
    float attackCooldown;
    float timeSinceLastAttack;
    int attackCounter;  // For attack cycling
    
    // Movement methods
    void updateBossState(float deltaTime, Vector2 playerPos);
    void updateMovement(float deltaTime, Vector2 playerPos);
    Vector2 calculateCirclingPosition(Vector2 playerPos, float angle, float radius);
    
    // Attack methods
    void executeAttack(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets);
    void singleShotAttack(Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets);
    void spreadShotAttack(Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets);
    void circularShotAttack(Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets);
    
    // Visualization
    void renderHealthBar(SDL_Renderer* renderer);
    void loadBossSprites(SDL_Renderer* renderer);
    
    SDL_Texture* bossTexture;
};

// Factory function
std::unique_ptr<Enemy> CreateBossEnemy(const Vector2& pos, SDL_Renderer* renderer, int wave);
