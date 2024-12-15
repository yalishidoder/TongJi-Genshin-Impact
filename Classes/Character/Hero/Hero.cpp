/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: 主人公类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#include "Hero.h"
#include "Scene/MainScene.h"
#include "Scene/OtherScene.h"

Character::Character()
    : m_health(100)         // 初始生命值
    , m_sleepiness(100)     // 初始睡意值
    , m_attackPower(10)     // 初始攻击力
    , m_heroism(1000)       // 初始英雄度
    , m_speed(200.0f)       // 初始速度
    , m_ismale(0)       // 初始性别
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
    
    if (!m_ismale) {
        setTexture("Character/Hero/Animation/female/WALK_UP/WALK_UP_1.png");
    }
    else {
        setTexture("Character/Hero/Animation/male/male_sheet/WALK_UP_1.png");
    }
    setScale(0.8f); // 可以根据需要调整角色的缩放比例，默认设置为 1.0

    // 初始化角色的动画缓存
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");

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
        0.01f,  // 移动时间，可调整
        offset);
    runAction(moveAction);
}

// 创建向上行走动画
cocos2d::Animation* Character::createWalkUpAnimation() {
    if (!m_ismale) {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_UP.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_UP.png");
    }
    else {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_UP.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_UP.png");
    }
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("WALK_UP_%d.png", i);
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
cocos2d::Animation* Character::createWalkDownAnimation() {
    if (!m_ismale) {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_DOWN.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_DOWN.png");
    }
    else {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_DOWN.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_DOWN.png");
    }
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;
    
    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("WALK_DOWN_%d.png", i);
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
cocos2d::Animation* Character::createWalkLeftAnimation() {
    if (!m_ismale) {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_LEFT.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_LEFT.png");
    }
    else {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_LEFT.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_LEFT.png");
    }
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("WALK_LEFT_%d.png", i);
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
cocos2d::Animation* Character::createWalkRightAnimation() {
    if (!m_ismale) {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_RIGHT.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/WALK_RIGHT.png");
    }
    else {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_RIGHT.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/WALK_RIGHT.png");
    }
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("WALK_RIGHT_%d.png", i);
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
void Character::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);
        
    }
}

// 停止当前动画
void Character::stopAnimation() {
    //stopAllActionsByTag(100);  // 这里用100作为动画相关action的标签，可自定义
    stopAllActions();
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// 添加动画到缓存
void Character::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
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

// 获取性别
bool Character::getGender() {
    return m_ismale;
}

// 设置性别
void Character::setGender(bool is_male) {
    m_ismale = is_male;
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
        // 播放向上行走动画并设置为循环播放
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_up_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_up_hero");
        }
    }
    else {
        // 停止向上行走动画
        if (m_currentAnimate && m_animationCache->getAnimation("walk_up_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    if (m_moveDown) {
        m_moveDirection.y -= m_speed * dt;
        // 播放向下行走动画并设置为循环播放
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_down_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_down_hero");
        }
    }
    else {
        // 停止向下行走动画
        if (m_currentAnimate && m_animationCache->getAnimation("walk_down_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    if (m_moveLeft) {
        m_moveDirection.x -= m_speed * dt;
        // 播放向左行走动画并设置为循环播放
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_left_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_left_hero");
        }
    }
    else {
        // 停止向左行走动画
        if (m_currentAnimate && m_animationCache->getAnimation("walk_left_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    if (m_moveRight) {
        m_moveDirection.x += m_speed * dt;
        // 播放向右行走动画并设置为循环播放
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_right_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_right_hero");
        }
    }
    else {
        // 停止向右行走动画
        if (m_currentAnimate && m_animationCache->getAnimation("walk_right_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    //回退版
#if 0
    // 如果有移动方向，则先计算目标位置
    if (m_moveDirection != cocos2d::Vec2::ZERO) {
        // 计算目标位置
        cocos2d::Vec2 targetPosition = getPosition() + m_moveDirection;
        cocos2d::Vec2 beforePosition = getPosition();
        // 输出调试日志：角色的当前坐标、目标坐标
        CCLOG("Character update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
            getPosition().x, getPosition().y, targetPosition.x, targetPosition.y);

        // 获取当前场景
        cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
        if (currentScene) {
            // 检查目标位置是否碰到墙壁
            bool collision = false;
            // 判断场景类型，并调用不同的碰撞检测方法
            if (dynamic_cast<MainScene*>(currentScene)) {
                // 如果是 MainScene，使用 MainScene 的碰撞检测
                MainScene* scene = dynamic_cast<MainScene*>(currentScene);
                collision = scene->checkCollision(targetPosition);
            }
            else if (dynamic_cast<OtherScene*>(currentScene)) {
                // 如果是 OtherScene，使用 OtherScene 的碰撞检测
                OtherScene* scene = dynamic_cast<OtherScene*>(currentScene);
                collision = scene->checkCollision(targetPosition);
            }
            CCLOG("Collision check: Target Position = (%.2f, %.2f), Collision = %s",
                targetPosition.x, targetPosition.y, collision ? "True" : "False");

            if (!collision) {
                // 如果没有碰到墙壁，执行移动
                moveBy(m_moveDirection);
                CCLOG("Character moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
            }
            else {
                // 如果碰到墙壁，则退回一点，避免角色卡住
                cocos2d::Vec2 adjustedPosition = getPosition();
                const float ajt = 1;//退回的多少
                if (m_moveUp && collision) adjustedPosition.y -= ajt * m_speed * dt;
                if (m_moveDown && collision) adjustedPosition.y += ajt * m_speed * dt;
                if (m_moveLeft && collision) adjustedPosition.x += ajt * m_speed * dt;
                if (m_moveRight && collision) adjustedPosition.x -= ajt * m_speed * dt;

                // 输出退回后的新位置
                CCLOG("Character adjusted position due to collision: (%.2f, %.2f)", adjustedPosition.x, adjustedPosition.y);
                setPosition(adjustedPosition);
            }
        }
        else {
            CCLOG("MainScene not found");
        }
    }
#endif

    //不回退版，但如果角色一帧移动过多，则会超出过多，导致人物卡住（已解决）
#if 1
    // 如果有移动方向，则先计算目标位置
    if (m_moveDirection != cocos2d::Vec2::ZERO) {
        // 计算目标位置
        cocos2d::Vec2 targetPosition = getPosition() + m_moveDirection;

        // 输出调试日志：角色的当前坐标、目标坐标
        CCLOG("Character update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
            getPosition().x, getPosition().y, targetPosition.x, targetPosition.y);

        // 获取当前运行的场景
        cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
        if (currentScene) {
            // 检查目标位置是否碰到墙壁
            bool collision = false;

            // 判断场景类型并调用不同的碰撞检测方法
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

            // 如果没有碰撞，执行移动
            if (!collision) {
                moveBy(m_moveDirection);
                CCLOG("Character moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
            }
            else {
                // 如果碰撞发生，停止角色移动（不进行反向调整）
                CCLOG("Collision detected, stopping character move.");
                // 不改变角色的位置，让它停留在原地
                setPosition(getPosition());
            }
        }
        else {
            CCLOG("MainScene not found");
        }
    }
#endif

}

