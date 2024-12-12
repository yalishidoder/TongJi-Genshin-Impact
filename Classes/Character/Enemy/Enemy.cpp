/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.cpp
 * File Function: ������ʵ��
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"
#include "Enemy.h"
#include "../Hero/Hero.h"
#include <cmath>

Enemy::Enemy()
    : m_health(50)  // ��ʼ����ֵ
    , m_attackPower(5)  // ��ʼ������
    , m_isAlive(true)
    , m_moveSpeed(50.0f)  // ��ʼ�ƶ��ٶ�
    , isAttacking(false)  //�Ƿ������
    , currentState(EnemyState::PATROL)   //��ʼ״̬ΪѲ��
    ,dirX(1) ,dirY(1)     //��ʼѲ�߷�����Ϊ������
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

    spawnPoint = initPosition;

    // ������������ص��˵ĳ�ʼ��������
    // ���磺setTexture("enemy_default.png");
    
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
    //�ж���Χ�ڴ�������ٽ��в���
    
    if (isHeroExist(delta))
    {
        //aiLogic();
        //moveLogic(dt);
        //attackLogic();
    }
}


void Enemy::patrol(float delta)
{
    // Ѳ���߼�����������·���ƶ�������ƶ�
    // ʾ�����򵥵������ƶ�
    //�����������������Ѳ�ߵĹ���
    if (this->getPositionX() >= spawnPoint.x + rangedX)
    {
        dirX = -1;
    }
    else if (this->getPositionX() <= spawnPoint.x - rangedX)
    {
        dirX = 1;
    }
    
    // ����λ��
    this->setPositionX(this->getPosition().x + dirX * m_moveSpeed * delta);
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

void Enemy::setPlayer(Character* player)
{
    this->player = player;
}

void Enemy::setRadius(float radius = 100.0f)
{
    this->radius = radius;
}

//�ж���Χ���Ƿ�������
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
                // ִ��Ѳ���߼�
                patrol(dt);
                // �������Ƿ�����ⷶΧ
                if (distanceToPlayer <= radius)
                {
                    // �л���׷��״̬
                    currentState = EnemyState::CHASE;
                }
                break;

            case EnemyState::CHASE:
                // ׷����ҵ��߼�
                if (distanceToPlayer > radius)
                {
                    // �л�������״̬
                    currentState = EnemyState::RETURN;
                }
                else
                {
                    // ����׷�����
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
                // ���س�������߼�
                cocos2d::Vec2 directionToSpawn = spawnPoint - enemyPos;
                float distanceToSpawn = directionToSpawn.length();

                if (distanceToSpawn > 1.0f)
                {
                    directionToSpawn.normalize();
                    cocos2d::Vec2 movement = directionToSpawn * m_moveSpeed * dt;
                    this->setPosition(this->getPosition() + movement);

                    float angle = std::atan2(directionToSpawn.y, directionToSpawn.x) * (180.0f / M_PI);
                    this->setRotation(angle);

                    // �����ҽ����ⷶΧ�������л���׷��״̬
                    if (distanceToPlayer <= radius)
                    {
                        currentState = EnemyState::CHASE;
                    }
                }
                else
                {
                    // �Ѿ��ص������㣬�л���Ѳ��״̬
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