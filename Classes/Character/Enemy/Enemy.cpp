/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.cpp
 * File Function: ������ʵ��
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "Enemy.h"


Enemy::Enemy()
    : m_health(50)  // ��ʼ����ֵ
    , m_attackPower(5)  // ��ʼ������
    , m_isAlive(true)
    , m_moveSpeed(50.0f)  // ��ʼ�ƶ��ٶ�
{
}


Enemy::~Enemy() {
}


bool Enemy::init(const cocos2d::Vec2& initPosition) {
    if (!Sprite::init()) {
        return false;
    }


    // ���õ��˳�ʼλ��
    setPosition(initPosition);


    // ������������ص��˵ĳ�ʼ��������
    // ���磺setTexture("enemy_default.png");


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
        1.0f,  // �ƶ�ʱ�䣬�ɸ���ʵ���������
        targetPosition);
    runAction(moveAction);
}


void Enemy::moveBy(const cocos2d::Vec2& offset) {
    auto moveAction = cocos2d::MoveBy::create(
        1.0f,  // �ƶ�ʱ�䣬�ɵ���
        offset);
    runAction(moveAction);
}


void Enemy::attack() {
    // ���˵Ĺ����߼����ɸ�����Ҫ��չ�����緢���ӵ�����ս������
    // �������ʾ��������Ӿ���Ĺ���ʵ��
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
    // �򵥵�����ƶ��߼����ɸ�����Ҫ��չΪ׷����һ������߼�
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
    // �򵥵Ĺ����߼����ɸ�����Ҫ��չΪ�����ӵĹ����жϺʹ�������
    if (CCRANDOM_0_1() < 0.01) {  // 1% �ĸ��ʹ������ɵ���
        attack();
    }
}


void Enemy::aiLogic() {
    // �������ʵ�ָ����ӵ� AI �߼�������Ѳ�ߡ�׷����ҡ������ҵ�
    // ��ʱΪ�գ��ɸ�����Ϸ�������
}