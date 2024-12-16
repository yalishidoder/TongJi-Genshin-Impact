/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.cpp
 * File Function: ������ʵ��
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"
#include "Enemy.h"
#include "Character/CharacterBase.h"
#include "../Hero/Hero.h"
#include "Scene/MainScene.h"
#include "Scene/OtherScene.h"

#include <cmath>

Enemy::Enemy()
    : m_isAlive(true)
    , m_moveSpeed(50.0f)  // ��ʼ�ƶ��ٶ�
    , isAttacking(false)  //�Ƿ������
    , m_animationCache(cocos2d::AnimationCache::getInstance())
    , m_currentAnimate(nullptr)
    , currentState(EnemyState::PATROL)   //��ʼ״̬ΪѲ��
    ,dirX(1) ,dirY(1)     //��ʼѲ�߷�����Ϊ������
{
    setMaxHealth(50);
    setAttackPower(5);
}


Enemy::~Enemy() {
}


bool Enemy::init(const cocos2d::Vec2& initPosition) {
    if (!Sprite::init()) {
        return false;
    }


    // ���õ��˳�ʼλ��
    setPosition(initPosition);
    setScale(0.8f);
    spawnPoint = initPosition;

    // ������������ص��˵ĳ�ʼ��������
    // ���磺setTexture("enemy_default.png");
    
    setTexture("Character/Enemy/Animation/hitman/WALK_DOWN_0.png");
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_enemy");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_enemy");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_enemy");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_enemy");
    return true;
}


Enemy* Enemy::create(const cocos2d::Vec2& initPosition)  {
    auto enemy = new (std::nothrow) Enemy();
    if (enemy && enemy->init(initPosition)) {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

bool Enemy::isAlive() const
{
    return m_isAlive;
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
        //moveLogic(delta);
        //attackLogic();
    }
}


void Enemy::patrol(float delta)
{
    // Ѳ���߼�����������·���ƶ�������ƶ�
    // ʾ�����򵥵������ƶ�
    //�����������������Ѳ�ߵĹ���
   
    cocos2d::Vec2 moveDirection = cocos2d::Vec2(dirX * m_moveSpeed * delta, 0);
    cocos2d::Vec2 targetPosition = getPosition() + moveDirection;
    // ��ȡ��ǰ����������ײ���
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    bool collision = false;
    if (currentScene) {
        //��ȡ�Ƿ���ײ
        if (dynamic_cast<MainScene*>(currentScene)) {
            MainScene* scene = dynamic_cast<MainScene*>(currentScene);
            collision = scene->checkCollision(targetPosition);
        }
        else if (dynamic_cast<OtherScene*>(currentScene)) {
            OtherScene* scene = dynamic_cast<OtherScene*>(currentScene);
            collision = scene->checkCollision(targetPosition);
        }
        CCLOG("Collision check: Target Position = (%.2f, %.2f), Collision = %s",
            targetPosition.x, targetPosition.y, collision ? "True" : "False");

        //�����ұ߽�,��涨�����ƶ�
        if (this->getPositionX() >= spawnPoint.x + rangedX)
        {
            dirX = -1;
        }
        //С����߽磬��涨�����ƶ�
        else if (this->getPositionX() <= spawnPoint.x - rangedX)
        {
            dirX = 1;
        }

        // ���û����ײ��ִ���ƶ�
        if (!collision) {
            moveBy(moveDirection);
            CCLOG("enermy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
        }
        else {
            // ����ײ�������ƶ�
            if (dirX == 1) {
                // �����ƶ�����ǽ�ڣ���������
                dirX = -1;
            }
            else if (dirX == -1) {
                // �����ƶ�����ǽ�ڣ���������
                dirX = 1;
            }
        }
    }
}



void Enemy::attack() {
    // ���˵Ĺ����߼����ɸ�����Ҫ��չ�����緢���ӵ�����ս������
    // �������ʾ��������Ӿ���Ĺ���ʵ��
    CCLOG("Enemy attacks with power %d", m_attackPower);
}

bool Enemy::checkCollision(cocos2d::Sprite* target) {
    auto thisBoundingBox = getBoundingBox();
    auto targetBoundingBox = target->getBoundingBox();
    return thisBoundingBox.intersectsRect(targetBoundingBox);
}

void Enemy::setPlayer(Hero* player)
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
                    if (distanceToPlayer > 10)
                    {
                        directionToPlayer.normalize();
                        cocos2d::Vec2 movement = directionToPlayer * m_moveSpeed * dt;
                        // ���׷��ǰ�ĵ���λ��
                        CCLOG("Enemy position before move: (%.2f, %.2f)", this->getPositionX(), this->getPositionY());
                        // ������˵�Ŀ��λ��
                        cocos2d::Vec2 targetPosition = this->getPosition() + movement;

                        // ��ȡ��ǰ����������ײ���
                        cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
                        bool collision = false;
                        //����Ƿ���ײ
                        if (currentScene) {
                            if (dynamic_cast<MainScene*>(currentScene)) {
                                MainScene* scene = dynamic_cast<MainScene*>(currentScene);
                                collision = scene->checkCollision(targetPosition);
                            }
                            else if (dynamic_cast<OtherScene*>(currentScene)) {
                                OtherScene* scene = dynamic_cast<OtherScene*>(currentScene);
                                collision = scene->checkCollision(targetPosition);
                            }
                        }
                        // ���û������ǽ�ڣ�����׷��
                        if (!collision) {
                            this->setPosition(this->getPosition() + movement);
                            CCLOG("enermy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);

                        }
                        else {
                            // ����ǽ�ڣ����س�����
                            currentState = EnemyState::RETURN;
                            CCLOG("enermy touch the wall");
                            // ��ѡ����ǽ����Ч
                        }

                        float angle = std::atan2(directionToPlayer.y, directionToPlayer.x) * (180.0f / M_PI);
                        //this->setRotation(-angle);
                    }
                    else
                        currentState = EnemyState::STAY;
                }
                break;
            case EnemyState::STAY:
                // ֹͣ���߼�
                if (distanceToPlayer > 10)
                {
                    // �л���Ѳ��״̬
                    currentState = EnemyState::PATROL;
                }
                else
                {
                    //ATTACK
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
                    //this->setRotation(angle);

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

// �����������߶���
cocos2d::Animation* Enemy::createWalkUpAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_UP.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_UP.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_UP_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
cocos2d::Animation* Enemy::createWalkDownAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_DOWN.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_DOWN.png");
    
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_DOWN_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
cocos2d::Animation* Enemy::createWalkLeftAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_LEFT.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_LEFT.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_LEFT_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
cocos2d::Animation* Enemy::createWalkRightAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_RIGHT.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_RIGHT.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_RIGHT_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// ����ָ�����ƵĶ���
void Enemy::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);

    }
}

// ֹͣ��ǰ����
void Enemy::stopAnimation() {
    //stopAllActionsByTag(100);  // ������100��Ϊ�������action�ı�ǩ�����Զ���
    stopAllActions();
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// ��Ӷ���������
void Enemy::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
    // ����һ���µ� Animation ���󣬽������ animation ���Ƶ��¶�����
    auto newAnimation = Animation::create();
    newAnimation->setDelayPerUnit(animation.getDelayPerUnit());
    newAnimation->setLoops(animation.getLoops());
    newAnimation->setRestoreOriginalFrame(animation.getRestoreOriginalFrame());
    for (const auto& frame : animation.getFrames()) {
        // �� AnimationFrame ����ȡ SpriteFrame �����
        SpriteFrame* spriteFrame = frame->getSpriteFrame();
        if (spriteFrame) {
            newAnimation->addSpriteFrame(spriteFrame);
        }
    }
    m_animationCache->addAnimation(newAnimation, animationName);
}