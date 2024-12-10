/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.cpp
 * File Function: 敌人类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "Enemy.h"


Enemy::Enemy()
    : m_health(50)  // 初始生命值
    , m_attackPower(5)  // 初始攻击力
    , m_isAlive(true)
    , m_moveSpeed(50.0f)  // 初始移动速度
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


    // 可以在这里加载敌人的初始外观纹理等
    // 例如：setTexture("enemy_default.png");


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


void Enemy::update(float dt) {
    aiLogic();
    moveLogic(dt);
    attackLogic();
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