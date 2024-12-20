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
    setMaxHealth(100);
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
    damageLabel = cocos2d::Label::createWithTTF("", "fonts/arial.ttf", 24);
    cocos2d::Vec2 labelPos = this->getAnchorPoint();
    labelPos.x += 30;
    labelPos.y += 60;
    damageLabel->setPosition(labelPos);
    this->addChild(damageLabel);
    damageLabel->setVisible(false);

    setTexture("Character/Enemy/Animation/hitman/WALK_DOWN_0.png");
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_enemy");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_enemy");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_enemy");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_enemy");
    m_animationCache->addAnimation(createDeathAnimation(), "death_enemy");

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
Hero* Enemy::getPlayer()
{
    return player;
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

void Enemy::setInitData(int level)
{
    this->setLevel(level);
    this->setMaxHealth(std::pow(1.2,(level + 1))*100);
    this->setHealth(this->getMaxHealth());
    this->setAttackPower(std::pow(1.2,level + 1)*50);
}

void Enemy::setDeath() {
    if (!m_isAlive) return;

    m_isAlive = false;
    currentState = EnemyState::STAY;

#if 1
    // ������������
    playAnimation("death_enemy");

    // ʹ���ӳٶ������ӳ��Ƴ�����
    auto delay = cocos2d::DelayTime::create(1.0f);
    auto remove = cocos2d::CallFunc::create([=]() {
        removeFromParentAndCleanup(true);
        });
    auto sequence = cocos2d::Sequence::create(delay, remove, nullptr);
    runAction(sequence);
    
#endif
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

// �򵥵�Ԫ�ط�Ӧ�߼�
// ͬԪ������ 
// ����ڻ� ˮ������ ��������ԭ��Ĺ���
// ����ģ���Ԫ�ش���������Ԫ�ػ�����Ԫ�ضܣ�������Լ򻯳���Ԫ��ʼ�ռ���20%
// ˮ�������ᶳ�� ��ֻ�ڱ���������ˮԪ�صĽ�ɫ�������
// �ڶ��������£��ܵ���Ԫ�ع��������

void Enemy::takeDamage(float damage)
{
    if (!m_isAlive)
        return;
    Hero* player = this->getPlayer();

    switch (player->getElement())//Ԫ�ظ����˺�
    {
    case(CharacterElement::FIRE):
        CCLOG("BURN!");
        //��Ч��
        switch (this->getElement())
        {
        case(CharacterElement::ICE):
            damage *= 2.0f;
            break;
        case(CharacterElement::WATER):
            damage *= 1.5f;
            break;
        case(CharacterElement::ROCK):
            damage *= 0.8f;
            break;
        default:
            break;
        }
        break;
    case(CharacterElement::ICE):
        CCLOG("FREEZE!");
        //��Ч��
        switch (this->getElement())
        {
        case(CharacterElement::FIRE):
            damage *= 1.5f;
            break;
        case(CharacterElement::WATER):
            // freeze()
            break;
        case(CharacterElement::ROCK):
            damage *= 0.8f;
            break;
        default:
            break;
        }
        break;
    case(CharacterElement::WATER):
        CCLOG("BE WATER MY FRIEND");
        //ˮЧ��
        switch (this->getElement())
        {
        case(CharacterElement::FIRE):
            damage *= 2.0f;
            break;
        case(CharacterElement::ICE):
            // freeze()
            break;
        case(CharacterElement::ROCK):
            damage *= 0.8f;
            break;
        default:
            break;
        }
        break;
    case(CharacterElement::ROCK):
        CCLOG("YOU SHALL NOT PASS!");
        //��Ч��
        //if(freeze())
        //   damage *= 3;
        break;
    default:
        break;
    }

    if (this->getElement() == player->getElement())
        damage = 0;

    // ���ٵ��˵�����ֵ
    float health = this->getHealth() - damage;

    const int player_lv = player->getLevel();
    const int enemy_lv = this->getLevel();
    const int lv_gap = (enemy_lv - player_lv) < 5 ? std::pow(1.2, enemy_lv - player_lv + 1) : enemy_lv - player_lv;    // �ȼ����ֲ�

    // �����˺�������ʾ
    updateDamageLabel(damage);
    player->addExp(log10(damage + 10) * lv_gap / 10 + 10);  // �˺�����

    // �������Ƿ�����    
    if (health < 0) {
        player->addExp(std::pow(1.2, lv_gap) * 5 + 50);  // ��ɱ�ȼ�����
        this->setDeath();
    }
    else
        this->setHealth(health);


}

// �����˺�������ʾ
void Enemy::updateDamageLabel(int damage)
{

    damageLabel->setString('-'+std::to_string(damage));
    damageLabel->setVisible(true);
    // ��ʼ��ɫ
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    damageLabel->setTextColor(initialColor);

    // ��˸��ɫ
    cocos2d::Color4B blinkColor = cocos2d::Color4B::BLACK;
    float blinkInterval = 0.05f;
    int blinkTimes = 1;

    // ��˸����
    cocos2d::ActionInterval* blinkAction = cocos2d::Repeat::create(cocos2d::Sequence::create(
        cocos2d::TintTo::create(blinkInterval, blinkColor.r, blinkColor.g, blinkColor.b),
        cocos2d::TintTo::create(blinkInterval, initialColor.r, initialColor.g, initialColor.b),
        nullptr
    ), blinkTimes);

    // Ŀ����ɫ
    cocos2d::Color3B targetColor = cocos2d::Color3B::RED;

    // ��ɫ���䶯��
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // �������ض���
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            damageLabel->setVisible(false);
            }),
        nullptr
    );

    // ���ж���
    damageLabel->runAction(hideAction);
}

// ��ȡ����Ԫ��
CharacterElement Enemy::getElement()
{
    return element;
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
                patrolLogic(dt, distanceToPlayer);
                break;
            case EnemyState::CHASE:
                chaseLogic(dt, playerPos, enemyPos, directionToPlayer, distanceToPlayer);
                break;
            case EnemyState::STAY:
                stayLogic(distanceToPlayer);
                break;
            case EnemyState::RETURN:
                returnToSpawnLogic(dt, enemyPos, distanceToPlayer, playerPos);
                break;
        }
        return true;
    }
    return false;
}

void Enemy::patrolLogic(float& dt, float& distanceToPlayer)
{
    patrol(dt);

    if (distanceToPlayer <= radius)
    {
        currentState = EnemyState::CHASE;
    }
}

void Enemy::chaseLogic(float& dt, cocos2d::Vec2& playerPos, cocos2d::Vec2& enemyPos, cocos2d::Vec2& directionToPlayer, float& distanceToPlayer)
{
    if (distanceToPlayer > radius)
    {
        currentState = EnemyState::RETURN;
    }
    else
    {
        if (distanceToPlayer > 10)
        {
            handleChasingMovement(dt, directionToPlayer);
        }
        else
        {
            currentState = EnemyState::STAY;
        }
    }
}

void Enemy::stayLogic(float& distanceToPlayer)
{
    if (distanceToPlayer > 10 && m_isAlive)
    {
        currentState = EnemyState::PATROL;
    }
    else
    {
        // �������߼�
    }
}

void Enemy::returnToSpawnLogic(float& dt, cocos2d::Vec2& enemyPos, float& distanceToPlayer, cocos2d::Vec2& playerPos)
{
    handleReturnToSpawnMovement(dt, enemyPos);

    if (distanceToPlayer <= radius)
    {
        currentState = EnemyState::CHASE;
    }
}

void Enemy::handleChasingMovement(float& dt, cocos2d::Vec2& directionToPlayer)
{
    directionToPlayer.normalize();
    cocos2d::Vec2 movement = directionToPlayer * m_moveSpeed * dt;

    CCLOG("Enemy position before move: (%.2f, %.2f)", this->getPositionX(), this->getPositionY());
    cocos2d::Vec2 targetPosition = this->getPosition() + movement;

    bool collision = checkCollisionWithScene(targetPosition);

    if (!collision)
    {
        this->setPosition(this->getPosition() + movement);
        CCLOG("Enemy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
    }
    else
    {
        currentState = EnemyState::RETURN;
        CCLOG("Enemy touched the wall");
    }

    float angle = std::atan2(directionToPlayer.y, directionToPlayer.x) * (180.0f / M_PI);
    //this->setRotation(-angle);
}

void Enemy::handleReturnToSpawnMovement(float& dt, cocos2d::Vec2& enemyPos)
{
    cocos2d::Vec2 directionToSpawn = spawnPoint - enemyPos;
    float distanceToSpawn = directionToSpawn.length();

    if (distanceToSpawn > 1.0f)
    {
        directionToSpawn.normalize();
        cocos2d::Vec2 movement = directionToSpawn * m_moveSpeed * dt;
        this->setPosition(this->getPosition() + movement);

        float angle = std::atan2(directionToSpawn.y, directionToSpawn.x) * (180.0f / M_PI);
        //this->setRotation(angle);
    }
    else
    {
        currentState = EnemyState::PATROL;
        setRotation(0.0f);
        dirX = 1;
    }
}

bool Enemy::checkCollisionWithScene(cocos2d::Vec2& targetPosition)
{
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    if (currentScene)
    {
        if (dynamic_cast<MainScene*>(currentScene))
        {
            MainScene* scene = dynamic_cast<MainScene*>(currentScene);
            return scene->checkCollision(targetPosition);
        }
        else if (dynamic_cast<OtherScene*>(currentScene))
        {
            OtherScene* scene = dynamic_cast<OtherScene*>(currentScene);
            return scene->checkCollision(targetPosition);
        }
    }
    return false;
}

void Enemy::patrol(float delta)
{
    // Ѳ���߼�����������·���ƶ�������ƶ�
    // ʾ�����򵥵������ƶ�
    //�����������������Ѳ�ߵĹ���

    cocos2d::Vec2 moveDirection = cocos2d::Vec2(dirX * m_moveSpeed * delta, 0);
    cocos2d::Vec2 targetPosition = getPosition() + moveDirection;

    // ��ȡ��ǰ����������ײ���
    bool collision = checkCollisionWithScene(targetPosition);

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
    if (!collision)
    {
        moveBy(moveDirection);
        CCLOG("Enemy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
    }
    else // ����ײ�������ƶ�
    {
        // �����ƶ�����ǽ�ڣ���������
        if (dirX == 1)
        {
            dirX = -1;
        }
        // �����ƶ�����ǽ�ڣ���������
        else if (dirX == -1)
        {
            dirX = 1;
        }
    }
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

// ������������
cocos2d::Animation* Enemy::createDeathAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/ENEMY_DEATH.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/ENEMY_DEATH.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 0; i <= 6; ++i) {
        std::string frameName = StringUtils::format("DEATH-%d.png", i);
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