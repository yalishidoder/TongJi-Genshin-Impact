/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: ���˹���ʵ��
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#include "Hero.h"

Character::Character()
    : m_health(100)         // ��ʼ����ֵ
    , m_sleepiness(100)     // ��ʼ˯��ֵ
    , m_attackPower(10)     // ��ʼ������
    , m_heroism(1000)       // ��ʼӢ�۶�
    , m_speed(100.0f)       // ��ʼ�ٶ�
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
    // ���ý�ɫ�ĳ�ʼλ��
    //setPosition(initPosition);

    // ���ؽ�ɫ�ĳ�ʼ����
    setTexture("Character/Hero/test.png"); // ʹ�� "test.png" ��Ϊ��ɫ�Ĳ�������
    setScale(1.0f); // ���Ը�����Ҫ������ɫ�����ű�����Ĭ������Ϊ 1.0

    // ��ʼ����ɫ�Ķ�������
    //m_animationCache = cocos2d::AnimationCache::getInstance();
    //m_animationCache->addAnimation(createIdleAnimation(), "idle_animation");
    //m_animationCache->addAnimation(createRunAnimation(), "run_animation");
    //m_animationCache->addAnimation(createAttackAnimation(), "attack_animation");
    //m_animationCache->addAnimation(createHurtAnimation(), "hurt_animation");
    //m_animationCache->addAnimation(createDieAnimation(), "die_animation");

    return true;
}

// �ƶ���ָ��λ��
void Character::moveTo(const cocos2d::Vec2& targetPosition) {
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // �ƶ�ʱ�䣬�ɸ���ʵ���������
        targetPosition);
    runAction(moveAction);
}

// ����ƫ�����ƶ�
void Character::moveBy(const cocos2d::Vec2& offset) {
    auto moveAction = cocos2d::MoveBy::create(
        0.1f,  // �ƶ�ʱ�䣬�ɵ���
        offset);
    runAction(moveAction);
}

// ����ָ�����ƵĶ���
void Character::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);
    }
}

// ֹͣ��ǰ����
void Character::stopAnimation() {
    stopAllActionsByTag(100);  // ������100��Ϊ�������action�ı�ǩ�����Զ���
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// ��Ӷ���������
void Character::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
    //m_animationCache->addAnimation(animation, animationName);
}

// ��������ֵ
void Character::setHealth(int health) {
    m_health = health;
    if (m_health <= 0) {
        m_isAlive = false;
    }
}

// ��ȡ����ֵ
int Character::getHealth() {
    return m_health;
}

// ���ù�����
void Character::setAttackPower(int attackPower) {
    m_attackPower = attackPower;
}

// ��ȡ������
int Character::getAttackPower() {
    return m_attackPower;
}

bool Character::isAlive() {
    return m_isAlive;
}

bool Character::checkCollision(Character* otherCharacter) {
    // ��ȡ��ɫ�ı߽��
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

