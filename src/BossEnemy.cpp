#include "BossEnemy.h"
#include "Bullet.h"
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

BossEnemy::BossEnemy(Vector2 pos, SDL_Renderer* renderer, int currentWave)
    : Enemy(pos, renderer), spawnWave(currentWave), currentState(BossState::HUNTING),
      stateTimer(0.0f), stateDuration(1.0f), targetPosition(pos),
      currentAttackType(BossAttackType::SINGLE_SHOT), attackCooldown(0.3f),
      timeSinceLastAttack(0.0f), attackCounter(0), bossTexture(nullptr) {
    
    // Scale characteristics based on wave
    maxHealth = 300 + (currentWave * 75);  // 375 on wave 1, 450 on wave 2, etc.
    health = maxHealth;
    radius = 60.0f;    // Larger than regular enemy (30.0f)
    speed = 130.0f;     // Very fast
    damage = 25;       // High damage compared to regular enemies (10)
    
    loadBossSprites(renderer);
    
    std::cout << "Boss spawned on wave " << currentWave << " with " << maxHealth << " health" << std::endl;
}

BossEnemy::~BossEnemy() {
    if (bossTexture) {
        SDL_DestroyTexture(bossTexture);
    }
}

void BossEnemy::loadBossSprites(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/enemies/rix.png");
    if (!surface) {
        std::cout << "Failed to load rix.png: " << IMG_GetError() << std::endl;
        bossTexture = nullptr;
        return;
    }
    bossTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!bossTexture) {
        std::cout << "Failed to create boss texture: " << SDL_GetError() << std::endl;
    }
}

void BossEnemy::updateBossState(float deltaTime, Vector2 playerPos) {
    stateTimer += deltaTime;
    
    BossState previousState = currentState;
    
    switch (currentState) {
        case BossState::HUNTING:
            if (stateTimer >= stateDuration) {
                currentState = BossState::CIRCLING;
                stateTimer = 0.0f;
                stateDuration = 2.0f;
            }
            break;
            
        case BossState::CIRCLING:
            if (stateTimer >= stateDuration) {
                currentState = BossState::ATTACKING;
                stateTimer = 0.0f;
                stateDuration = attackCooldown;  // Match attackCooldown for single shot at end
                timeSinceLastAttack = 0.0f;  // Reset attack timer when entering ATTACKING state
            }
            break;
            
        case BossState::ATTACKING:
            if (stateTimer >= stateDuration) {
                currentState = BossState::RETREATING;
                stateTimer = 0.0f;
                stateDuration = 1.0f;
            }
            break;
            
        case BossState::RETREATING:
            if (stateTimer >= stateDuration) {
                currentState = BossState::HUNTING;
                stateTimer = 0.0f;
                stateDuration = 1.0f;
            }
            break;
    }
    
    // Log state changes
    if (previousState != currentState) {
        const char* stateNames[] = {"HUNTING", "CIRCLING", "ATTACKING", "RETREATING"};
        std::cout << "Boss: " << stateNames[static_cast<int>(previousState)] 
                  << " -> " << stateNames[static_cast<int>(currentState)] << std::endl;
    }
}

void BossEnemy::updateMovement(float deltaTime, Vector2 playerPos) {
    Vector2 direction;
    
    switch (currentState) {
        case BossState::HUNTING:
            // Direct movement toward player
            direction = (playerPos - position).normalized();
            velocity = direction * speed;
            break;
            
        case BossState::CIRCLING: {
            // Circular movement around player
            float angle = stateTimer * 1.5f; // Rotation speed
            float radius = 150.0f;
            targetPosition = calculateCirclingPosition(playerPos, angle, radius);
            direction = (targetPosition - position).normalized();
            velocity = direction * speed;
            break;
        }
        
        case BossState::ATTACKING:
            // Slows down during attack
            velocity = velocity * 0.3f;
            break;
            
        case BossState::RETREATING:
            // Moves away from player
            direction = (position - playerPos).normalized();
            velocity = direction * speed * 0.7f;
            break;
    }
    
    position += velocity * deltaTime;
}

Vector2 BossEnemy::calculateCirclingPosition(Vector2 playerPos, float angle, float radius) {
    return Vector2(
        playerPos.x + cos(angle) * radius,
        playerPos.y + sin(angle) * radius
    );
}

void BossEnemy::executeAttack(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    timeSinceLastAttack += deltaTime;
    
    if (timeSinceLastAttack < attackCooldown) {
        return;
    }
    
    // Cycle through attack types
    switch (attackCounter % 3) {
        case 0:
            currentAttackType = BossAttackType::SINGLE_SHOT;
            break;
        case 1:
            currentAttackType = BossAttackType::SPREAD_SHOT;
            break;
        case 2:
            currentAttackType = BossAttackType::CIRCULAR_SHOT;
            break;
    }
    
    switch (currentAttackType) {
        case BossAttackType::SINGLE_SHOT:
            singleShotAttack(playerPos, bullets);
            break;
        case BossAttackType::SPREAD_SHOT:
            spreadShotAttack(playerPos, bullets);
            break;
        case BossAttackType::CIRCULAR_SHOT:
            circularShotAttack(playerPos, bullets);
            break;
    }
    
    timeSinceLastAttack = 0.0f;
    attackCounter++;
}

void BossEnemy::singleShotAttack(Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    Vector2 direction = (playerPos - position).normalized();
    bullets.push_back(std::make_unique<Bullet>(
        position, direction, 
        /*damage*/ 15, /*range*/ 800.0f, /*speed*/ 400.0f, 
        BulletType::BOSS_LARGE, /*enemyOwned*/ true
    ));
}

void BossEnemy::spreadShotAttack(Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    Vector2 baseDirection = (playerPos - position).normalized();
    
    // 5 bullets in spread pattern (angles: -30°, -15°, 0°, 15°, 30°)
    for (int i = 0; i < 5; i++) {
        float angleOffset = (i - 2) * 0.2617f; // 15° in radians
        Vector2 direction(
            baseDirection.x * cos(angleOffset) - baseDirection.y * sin(angleOffset),
            baseDirection.x * sin(angleOffset) + baseDirection.y * cos(angleOffset)
        );
        
        bullets.push_back(std::make_unique<Bullet>(
            position, direction,
            /*damage*/ 12, /*range*/ 600.0f, /*speed*/ 350.0f,
            BulletType::BOSS_MEDIUM, /*enemyOwned*/ true
        ));
    }
}

void BossEnemy::circularShotAttack(Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    // 8 bullets in circle (every 45°)
    for (int i = 0; i < 8; i++) {
        float angle = i * 0.7854f; // 45° in radians
        Vector2 direction(cos(angle), sin(angle));
        
        bullets.push_back(std::make_unique<Bullet>(
            position, direction,
            /*damage*/ 10, /*range*/ 500.0f, /*speed*/ 300.0f,
            BulletType::BOSS_SMALL, /*enemyOwned*/ true
        ));
    }
}

void BossEnemy::renderHealthBar(SDL_Renderer* renderer) {
    // Health bar dimensions
    int barWidth = 120;
    int barHeight = 12;
    int barX = static_cast<int>(position.x - static_cast<float>(barWidth) / 2.0f);
    int barY = static_cast<int>(position.y - radius - 20);
    
    // Background bar (Yellow)
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

void BossEnemy::render(SDL_Renderer* renderer) {
    // Render boss sprite
    if (bossTexture) {
        SDL_Rect destRect = {
            static_cast<int>(position.x - radius),
            static_cast<int>(position.y - radius),
            static_cast<int>(radius * 2),
            static_cast<int>(radius * 2)
        };
        SDL_RenderCopy(renderer, bossTexture, nullptr, &destRect);
    } else {
        // Fallback: red circle
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        int centerX = static_cast<int>(position.x);
        int centerY = static_cast<int>(position.y);
        int r = static_cast<int>(radius);
        
        for (int y = -r; y <= r; y++) {
            for (int x = -r; x <= r; x++) {
                if (x*x + y*y <= r*r) {
                    SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                }
            }
        }
    }
    
    // Render health bar
    renderHealthBar(renderer);
}

void BossEnemy::update(float deltaTime, Vector2 playerPos, std::vector<std::unique_ptr<Bullet>>& bullets) {
    updateBossState(deltaTime, playerPos);
    updateMovement(deltaTime, playerPos);
    
    // Attacks only in ATTACKING state
    if (currentState == BossState::ATTACKING) {
        executeAttack(deltaTime, playerPos, bullets);
    }
    
    // Base animation from parent class
    animationTimer += deltaTime;
    if (state == EnemyState::HIT) {
        hitTimer += deltaTime;
        if (hitTimer > 0.3f) { // Show hit effect longer
            state = EnemyState::IDLE;
            hitTimer = 0.0f;
        }
    }
}

void BossEnemy::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        alive = false;
        std::cout << "Boss defeated!" << std::endl;
    } else {
        std::cout << "Boss health: " << health << "/" << maxHealth << std::endl;
    }
    hit(); // Show hit animation
}

// Factory function
std::unique_ptr<Enemy> CreateBossEnemy(const Vector2& pos, SDL_Renderer* renderer, int wave) {
    return std::make_unique<BossEnemy>(pos, renderer, wave);
}
