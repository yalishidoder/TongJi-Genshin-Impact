/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: 主人公类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#include "Hero.h"

Character::Character()
    : m_health(100)         // 初始生命值
    , m_sleepiness(100)     // 初始睡意值
    , m_attackPower(10)     // 初始攻击力
    , m_heroism(1000)       // 初始英雄度
    , m_speed(100.0f)       // 初始速度
    , m_isAlive(true)
    , m_animationCache(cocos2d::AnimationCache::getInstance())
    , m_currentAnimate(nullptr)
    , m_moveDirection(cocos2d::Vec2::ZERO)
    , m_moveUp(false)
    , m_moveDown(false)
    , m_moveLeft(false)
    , m_moveRight(false) {
}

Character::~Character() {
    CC_SAFE_RELEASE_NULL(m_animationCache);
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

Character* Character::create(const cocos2d::Vec2& initPosition) {
    auto character = new (std::nothrow) Character();
    if (character && character->init(initPosition)) {
        character->autorelease();
        return character;
    }
    CC_SAFE_DELETE(character);
    return nullptr;
}

bool Character::init(const cocos2d::Vec2& initPosition) {
    if (!Sprite::init()) {
        return false;
    }
    // 设置角色的初始位置
    //setPosition(initPosition);

    // 加载角色的初始纹理
    setTexture("Character/Hero/test.png"); // 使用 "test.png" 作为角色的测试纹理
    setScale(1.0f); // 可以根据需要调整角色的缩放比例，默认设置为 1.0

    // 初始化角色的动画缓存
    //m_animationCache = cocos2d::AnimationCache::getInstance();
    //m_animationCache->addAnimation(createIdleAnimation(), "idle_animation");
    //m_animationCache->addAnimation(createRunAnimation(), "run_animation");
    //m_animationCache->addAnimation(createAttackAnimation(), "attack_animation");
    //m_animationCache->addAnimation(createHurtAnimation(), "hurt_animation");
    //m_animationCache->addAnimation(createDieAnimation(), "die_animation");

    return true;
}

// 移动到指定位置
void Character::moveTo(const cocos2d::Vec2& targetPosition) {
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // 移动时间，可根据实际情况调整
        targetPosition);
    runAction(moveAction);
}

// 按照偏移量移动
void Character::moveBy(const cocos2d::Vec2& offset) {
    auto moveAction = cocos2d::MoveBy::create(
        0.1f,  // 移动时间，可调整
        offset);
    runAction(moveAction);
}

// 播放指定名称的动画
void Character::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);
    }
}

// 停止当前动画
void Character::stopAnimation() {
    stopAllActionsByTag(100);  // 这里用100作为动画相关action的标签，可自定义
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// 添加动画到缓存
void Character::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
    //m_animationCache->addAnimation(animation, animationName);
}

// 设置生命值
void Character::setHealth(int health) {
    m_health = health;
    if (m_health <= 0) {
        m_isAlive = false;
    }
}

// 获取生命值
int Character::getHealth() {
    return m_health;
}

// 设置攻击力
void Character::setAttackPower(int attackPower) {
    m_attackPower = attackPower;
}

// 获取攻击力
int Character::getAttackPower() {
    return m_attackPower;
}

bool Character::isAlive() {
    return m_isAlive;
}

bool Character::checkCollision(Character* otherCharacter) {
    // 获取角色的边界框
    auto thisBoundingBox = getBoundingBox();
    auto otherBoundingBox = otherCharacter->getBoundingBox();
    return thisBoundingBox.intersectsRect(otherBoundingBox);
}

void Character::update(float dt) {
    m_moveDirection = cocos2d::Vec2::ZERO;


    if (m_moveUp) {
        m_moveDirection.y += m_speed * dt;
    }
    if (m_moveDown) {
        m_moveDirection.y -= m_speed * dt;
    }
    if (m_moveLeft) {
        m_moveDirection.x -= m_speed * dt;
    }
    if (m_moveRight) {
        m_moveDirection.x += m_speed * dt;
    }


    if (m_moveDirection != cocos2d::Vec2::ZERO) {
        moveBy(m_moveDirection);
    }
}

