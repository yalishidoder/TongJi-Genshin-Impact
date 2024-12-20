/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.cpp
 * File Function: 敌人类实现
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
    , m_moveSpeed(50.0f)  // 初始移动速度
    , isAttacking(false)  //是否发起进攻
    , m_animationCache(cocos2d::AnimationCache::getInstance())
    , m_currentAnimate(nullptr)
    , currentState(EnemyState::PATROL)   //初始状态为巡逻
    ,dirX(1) ,dirY(1)     //初始巡逻方向设为正方向
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


    // 设置敌人初始位置
    setPosition(initPosition);
    setScale(0.8f);
    spawnPoint = initPosition;

    // 可以在这里加载敌人的初始外观纹理等
    // 例如：setTexture("enemy_default.png");
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
    // 播放死亡动画
    playAnimation("death_enemy");

    // 使用延迟动作来延迟移除敌人
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
    //判定范围内存在玩家再进行操作
    
    if (isHeroExist(delta))
    {
        //aiLogic();
        //moveLogic(delta);
        //attackLogic();
    }

}

// 简单的元素反应逻辑
// 同元素无伤 
// 火冰融化 水火蒸发 倍率依照原神的规则
// 特殊的：岩元素触碰到其余元素会生成元素盾，这里可以简化成岩元素始终减伤20%
// 水冰相遇会冻结 但只在冰攻击附带水元素的角色的情况下
// 在冻结的情况下，受到岩元素攻击会碎冰

void Enemy::takeDamage(float damage)
{
    if (!m_isAlive)
        return;
    Hero* player = this->getPlayer();

    switch (player->getElement())//元素附加伤害
    {
    case(CharacterElement::FIRE):
        CCLOG("BURN!");
        //火效果
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
        //冰效果
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
        //水效果
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
        //岩效果
        //if(freeze())
        //   damage *= 3;
        break;
    default:
        break;
    }

    if (this->getElement() == player->getElement())
        damage = 0;

    // 减少敌人的生命值
    float health = this->getHealth() - damage;

    const int player_lv = player->getLevel();
    const int enemy_lv = this->getLevel();
    const int lv_gap = (enemy_lv - player_lv) < 5 ? std::pow(1.2, enemy_lv - player_lv + 1) : enemy_lv - player_lv;    // 等级差弥补

    // 更新伤害数字显示
    updateDamageLabel(damage);
    player->addExp(log10(damage + 10) * lv_gap / 10 + 10);  // 伤害经验

    // 检查敌人是否死亡    
    if (health < 0) {
        player->addExp(std::pow(1.2, lv_gap) * 5 + 50);  // 击杀等级经验
        this->setDeath();
    }
    else
        this->setHealth(health);


}

// 更新伤害数字显示
void Enemy::updateDamageLabel(int damage)
{

    damageLabel->setString('-'+std::to_string(damage));
    damageLabel->setVisible(true);
    // 初始颜色
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    damageLabel->setTextColor(initialColor);

    // 闪烁颜色
    cocos2d::Color4B blinkColor = cocos2d::Color4B::BLACK;
    float blinkInterval = 0.05f;
    int blinkTimes = 1;

    // 闪烁动作
    cocos2d::ActionInterval* blinkAction = cocos2d::Repeat::create(cocos2d::Sequence::create(
        cocos2d::TintTo::create(blinkInterval, blinkColor.r, blinkColor.g, blinkColor.b),
        cocos2d::TintTo::create(blinkInterval, initialColor.r, initialColor.g, initialColor.b),
        nullptr
    ), blinkTimes);

    // 目标颜色
    cocos2d::Color3B targetColor = cocos2d::Color3B::RED;

    // 颜色渐变动作
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // 最终隐藏动作
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            damageLabel->setVisible(false);
            }),
        nullptr
    );

    // 运行动作
    damageLabel->runAction(hideAction);
}

// 获取敌人元素
CharacterElement Enemy::getElement()
{
    return element;
}

void Enemy::attack() {
    // 敌人的攻击逻辑，可根据需要扩展，例如发射子弹、近战攻击等
    // 这里仅作示例，可添加具体的攻击实现
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
        // 处理攻击逻辑
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
    // 巡逻逻辑，例如沿着路径移动或随机移动
    // 示例：简单地左右移动
    //后续可以添加向上下巡逻的功能

    cocos2d::Vec2 moveDirection = cocos2d::Vec2(dirX * m_moveSpeed * delta, 0);
    cocos2d::Vec2 targetPosition = getPosition() + moveDirection;

    // 获取当前场景进行碰撞检测
    bool collision = checkCollisionWithScene(targetPosition);

    CCLOG("Collision check: Target Position = (%.2f, %.2f), Collision = %s",
        targetPosition.x, targetPosition.y, collision ? "True" : "False");

    //大于右边界,则规定向左移动
    if (this->getPositionX() >= spawnPoint.x + rangedX)
    {
        dirX = -1;
    }
    //小于左边界，则规定向右移动
    else if (this->getPositionX() <= spawnPoint.x - rangedX)
    {
        dirX = 1;
    }

    // 如果没有碰撞，执行移动
    if (!collision)
    {
        moveBy(moveDirection);
        CCLOG("Enemy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
    }
    else // 有碰撞，反向移动
    {
        // 向右移动碰到墙壁，反向向左
        if (dirX == 1)
        {
            dirX = -1;
        }
        // 向左移动碰到墙壁，反向向右
        else if (dirX == -1)
        {
            dirX = 1;
        }
    }
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

// 创建向上行走动画
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
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// 创建向下行走动画
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
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// 创建向左行走动画
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
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// 创建向右行走动画
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
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// 创建死亡动画
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
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// 播放指定名称的动画
void Enemy::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);

    }
}

// 停止当前动画
void Enemy::stopAnimation() {
    //stopAllActionsByTag(100);  // 这里用100作为动画相关action的标签，可自定义
    stopAllActions();
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// 添加动画到缓存
void Enemy::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
    // 创建一个新的 Animation 对象，将传入的 animation 复制到新对象中
    auto newAnimation = Animation::create();
    newAnimation->setDelayPerUnit(animation.getDelayPerUnit());
    newAnimation->setLoops(animation.getLoops());
    newAnimation->setRestoreOriginalFrame(animation.getRestoreOriginalFrame());
    for (const auto& frame : animation.getFrames()) {
        // 从 AnimationFrame 中提取 SpriteFrame 并添加
        SpriteFrame* spriteFrame = frame->getSpriteFrame();
        if (spriteFrame) {
            newAnimation->addSpriteFrame(spriteFrame);
        }
    }
    m_animationCache->addAnimation(newAnimation, animationName);
}