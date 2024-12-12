/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.cpp
 * File Function: 敌人类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"
#include "Enemy.h"
#include "../Hero/Hero.h"
#include <cmath>

Enemy::Enemy()
    : m_health(50)  // 初始生命值
    , m_attackPower(5)  // 初始攻击力
    , m_isAlive(true)
    , m_moveSpeed(50.0f)  // 初始移动速度
    , isAttacking(false)  //是否发起进攻
    , currentState(EnemyState::PATROL)   //初始状态为巡逻
    ,dirX(1) ,dirY(1)     //初始巡逻方向设为正方向
{
}


Enemy::~Enemy() {
}


bool Enemy::init(const cocos2d::Vec2& initPosition) {
    if (!Sprite::init()) {
        return false;
    }


    // 设置敌人初始位置
    setPosition(initPosition);

    spawnPoint = initPosition;

    // 可以在这里加载敌人的初始外观纹理等
    // 例如：setTexture("enemy_default.png");
    
    setTexture("Character/Enemy/test.png");

    return true;
}


Enemy* Enemy::create(const cocos2d::Vec2& initPosition) {
    auto enemy = new (std::nothrow) Enemy();
    if (enemy && enemy->init(initPosition)) {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

void Enemy::setPatrolRange(float X, float Y)
{
    this->rangedX = X;
    this->rangedY = Y;
}

void Enemy::update(float delta) 
{
    //判定范围内存在玩家再进行操作
    
    if (isHeroExist(delta))
    {
        //aiLogic();
        //moveLogic(dt);
        //attackLogic();
    }
}


void Enemy::patrol(float delta)
{
    // 巡逻逻辑，例如沿着路径移动或随机移动
    // 示例：简单地左右移动
    //后续可以添加向上下巡逻的功能
    if (this->getPositionX() >= spawnPoint.x + rangedX)
    {
        dirX = -1;
    }
    else if (this->getPositionX() <= spawnPoint.x - rangedX)
    {
        dirX = 1;
    }
    
    // 更新位置
    this->setPositionX(this->getPosition().x + dirX * m_moveSpeed * delta);
}

void Enemy::moveTo(const cocos2d::Vec2& targetPosition) {
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // 移动时间，可根据实际情况调整
        targetPosition);
    runAction(moveAction);
}


void Enemy::moveBy(const cocos2d::Vec2& offset) {
    auto moveAction = cocos2d::MoveBy::create(
        1.0f,  // 移动时间，可调整
        offset);
    runAction(moveAction);
}


void Enemy::attack() {
    // 敌人的攻击逻辑，可根据需要扩展，例如发射子弹、近战攻击等
    // 这里仅作示例，可添加具体的攻击实现
    CCLOG("Enemy attacks with power %d", m_attackPower);
}


void Enemy::setHealth(int health) {
    m_health = health;
    if (m_health <= 0) {
        m_isAlive = false;
    }
}


int Enemy::getHealth() {
    return m_health;
}


void Enemy::setAttackPower(int attackPower) {
    m_attackPower = attackPower;
}


int Enemy::getAttackPower() {
    return m_attackPower;
}


bool Enemy::isAlive() {
    return m_isAlive;
}


bool Enemy::checkCollision(cocos2d::Sprite* target) {
    auto thisBoundingBox = getBoundingBox();
    auto targetBoundingBox = target->getBoundingBox();
    return thisBoundingBox.intersectsRect(targetBoundingBox);
}

void Enemy::setPlayer(Character* player)
{
    this->player = player;
}

void Enemy::setRadius(float radius = 100.0f)
{
    this->radius = radius;
}

//判定范围内是否存在玩家
bool Enemy::isHeroExist(float dt)
{
    if (player)
    {
        cocos2d::Vec2 playerPos = player->getPosition();
        cocos2d::Vec2 enemyPos = this->getPosition();
        cocos2d::Vec2 directionToPlayer = playerPos - enemyPos;
        float distanceToPlayer = directionToPlayer.length();

        switch (currentState)
        {
            case EnemyState::PATROL:
                // 执行巡逻逻辑
                patrol(dt);
                // 检测玩家是否进入检测范围
                if (distanceToPlayer <= radius)
                {
                    // 切换到追踪状态
                    currentState = EnemyState::CHASE;
                }
                break;

            case EnemyState::CHASE:
                // 追踪玩家的逻辑
                if (distanceToPlayer > radius)
                {
                    // 切换到返回状态
                    currentState = EnemyState::RETURN;
                }
                else
                {
                    // 继续追踪玩家
                    if (distanceToPlayer > 1e-5f)
                    {
                        directionToPlayer.normalize();
                        cocos2d::Vec2 movement = directionToPlayer * m_moveSpeed * dt;
                        this->setPosition(this->getPosition() + movement);

                        float angle = std::atan2(directionToPlayer.y, directionToPlayer.x) * (180.0f / M_PI);
                        this->setRotation(-angle);
                    }
                }
                break;

            case EnemyState::RETURN:
                // 返回出生点的逻辑
                cocos2d::Vec2 directionToSpawn = spawnPoint - enemyPos;
                float distanceToSpawn = directionToSpawn.length();

                if (distanceToSpawn > 1.0f)
                {
                    directionToSpawn.normalize();
                    cocos2d::Vec2 movement = directionToSpawn * m_moveSpeed * dt;
                    this->setPosition(this->getPosition() + movement);

                    float angle = std::atan2(directionToSpawn.y, directionToSpawn.x) * (180.0f / M_PI);
                    this->setRotation(angle);

                    // 如果玩家进入检测范围，优先切换到追踪状态
                    if (distanceToPlayer <= radius)
                    {
                        currentState = EnemyState::CHASE;
                    }
                }
                else
                {
                    // 已经回到出生点，切换回巡逻状态
                    currentState = EnemyState::PATROL;
                    setRotation(0.0f);
                    dirX = 1;
                }
                break;
        }
    }

    return true;
}

void Enemy::moveLogic(float dt) {
    // 简单的随机移动逻辑，可根据需要扩展为追逐玩家或其他逻辑
    cocos2d::Vec2 moveDirection;
    float randomX = CCRANDOM_MINUS1_1();
    float randomY = CCRANDOM_MINUS1_1();
    moveDirection.x = randomX * m_moveSpeed * dt;
    moveDirection.y = randomY * m_moveSpeed * dt;


    if (moveDirection != cocos2d::Vec2::ZERO) {
        moveBy(moveDirection);
    }
}


void Enemy::attackLogic() {
    // 简单的攻击逻辑，可根据需要扩展为更复杂的攻击判断和触发条件
    if (CCRANDOM_0_1() < 0.01) {  // 1% 的概率攻击，可调整
        attack();
    }
}


void Enemy::aiLogic() {
    // 这里可以实现更复杂的 AI 逻辑，例如巡逻、追逐玩家、躲避玩家等
    // 暂时为空，可根据游戏需求添加

}