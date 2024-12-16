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
    setMaxHealth(50);
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
    //判定范围内存在玩家再进行操作
    
    if (isHeroExist(delta))
    {
        //aiLogic();
        //moveLogic(delta);
        //attackLogic();
    }
}


void Enemy::patrol(float delta)
{
    // 巡逻逻辑，例如沿着路径移动或随机移动
    // 示例：简单地左右移动
    //后续可以添加向上下巡逻的功能
   
    cocos2d::Vec2 moveDirection = cocos2d::Vec2(dirX * m_moveSpeed * delta, 0);
    cocos2d::Vec2 targetPosition = getPosition() + moveDirection;
    // 获取当前场景进行碰撞检测
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    bool collision = false;
    if (currentScene) {
        //读取是否碰撞
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
        if (!collision) {
            moveBy(moveDirection);
            CCLOG("enermy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
        }
        else {
            // 有碰撞，反向移动
            if (dirX == 1) {
                // 向右移动碰到墙壁，反向向左
                dirX = -1;
            }
            else if (dirX == -1) {
                // 向左移动碰到墙壁，反向向右
                dirX = 1;
            }
        }
    }
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
                    if (distanceToPlayer > 10)
                    {
                        directionToPlayer.normalize();
                        cocos2d::Vec2 movement = directionToPlayer * m_moveSpeed * dt;
                        // 输出追击前的敌人位置
                        CCLOG("Enemy position before move: (%.2f, %.2f)", this->getPositionX(), this->getPositionY());
                        // 计算敌人的目标位置
                        cocos2d::Vec2 targetPosition = this->getPosition() + movement;

                        // 获取当前场景进行碰撞检测
                        cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
                        bool collision = false;
                        //检测是否碰撞
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
                        // 如果没有碰到墙壁，继续追击
                        if (!collision) {
                            this->setPosition(this->getPosition() + movement);
                            CCLOG("enermy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);

                        }
                        else {
                            // 碰到墙壁，返回出生点
                            currentState = EnemyState::RETURN;
                            CCLOG("enermy touch the wall");
                            // 可选：碰墙后音效
                        }

                        float angle = std::atan2(directionToPlayer.y, directionToPlayer.x) * (180.0f / M_PI);
                        //this->setRotation(-angle);
                    }
                    else
                        currentState = EnemyState::STAY;
                }
                break;
            case EnemyState::STAY:
                // 停止的逻辑
                if (distanceToPlayer > 10)
                {
                    // 切换到巡逻状态
                    currentState = EnemyState::PATROL;
                }
                else
                {
                    //ATTACK
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
                    //this->setRotation(angle);

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