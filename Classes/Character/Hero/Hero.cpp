/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: 主人公类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#include <cmath>
#include "Hero.h"
#include "Character/CharacterBase.h"
#include "Weapon/RangedWeapon/Ammunition/Bullet.h"
#include "Weapon/MeleeWeapon/Bayonet.h"
#include "Scene/MainScene.h"
#include "Scene/OtherScene.h"


Hero::Hero()
    : m_sleepiness(100)     // 初始睡意值
    , m_heroism(1000)       // 初始英雄度
    , m_speed(200.0f)       // 初始速度
    , m_ismale(0)       // 初始性别
    , MaxLevel(100)         // 等级上限
    , m_exp(0)              // 初始经验值
    , m_expToLevelUp(100)   // 初始升级所需要的经验
    , m_isAlive(true)
    , m_animationCache(cocos2d::AnimationCache::getInstance())
    , m_currentAnimate(nullptr)
    , m_moveDirection(cocos2d::Vec2::ZERO)
    , m_moveUp(false)
    , m_moveDown(false)
    , m_moveLeft(false)
    , m_bayonet(nullptr)
    , m_moveRight(false) {
}

Hero::~Hero() {
    CC_SAFE_RELEASE_NULL(m_animationCache);
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

Hero* Hero::create(const cocos2d::Vec2& initPosition) {
    auto hero = new (std::nothrow) Hero();
    if (hero && hero->init(initPosition)) {
        hero->autorelease();
        return hero;
    }
    CC_SAFE_DELETE(hero);
    return nullptr;
}

bool Hero::init(const cocos2d::Vec2& initPosition) {
    if (!Sprite::init()) {
        return false;
    }
    // 加载角色的初始纹理
    if (!this->getGender()) {
        this->setTexture("Character/Hero/Animation/female/female_default.png");
    }
    else {
        this->setTexture("Character/Hero/Animation/male/male_default.png");
    }
    setScale(0.8f); // 调整角色的缩放比例，默认设置为 1.0

    // 初始化角色的动画缓存
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");

    return true;
}


// 移动到指定位置
void Hero::moveTo(const cocos2d::Vec2& targetPosition) {
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // 移动时间，可根据实际情况调整
        targetPosition);
    runAction(moveAction);
}

// 按照偏移量移动
void Hero::moveBy(const cocos2d::Vec2& offset) {
    auto moveAction = cocos2d::MoveBy::create(
        0.01f,  // 移动时间，可调整
        offset);
    runAction(moveAction);
}

// 创建向上行走动画
cocos2d::Animation* Hero::createWalkUpAnimation() {
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
cocos2d::Animation* Hero::createWalkDownAnimation() {
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
cocos2d::Animation* Hero::createWalkLeftAnimation() {
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
cocos2d::Animation* Hero::createWalkRightAnimation() {
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
void Hero::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);
        
    }
}

// 停止当前动画
void Hero::stopAnimation() {
    //stopAllActionsByTag(100);  // 这里用100作为动画相关action的标签，可自定义
    stopAllActions();
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// 添加动画到缓存
void Hero::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
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



void Hero::attack()
{
    //要与武器模块挂钩
}

void Hero::attackWithBullet(const Vec2& position)
{
    // 创建子弹
    auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO,this->getLevel());
    if (bullet) {

        // 加载子弹图形资源
        switch (this->getElement()) {
        case(CharacterElement::FIRE):
            bullet->setTexture("Weapon/Ammunition/bullet_fire.png");
            break;
        case(CharacterElement::ICE):
            bullet->setTexture("Weapon/Ammunition/bullet_ice.png");
            break;
        case(CharacterElement::WATER):
            bullet->setTexture("Weapon/Ammunition/bullet_water.png");
            break;
        case(CharacterElement::ROCK):
            bullet->setTexture("Weapon/Ammunition/bullet_rock.png");
            break;
        default:
            bullet->setTexture("Weapon/Ammunition/bullet_default.png");
            break;
        }

        // 设置锚点
        bullet->setAnchorPoint(Vec2(0.5f, 0.5f));

        // 设置子弹的位置为英雄的位置
        bullet->setPosition(getPosition());
        // 计算子弹的移动方向
        Vec2 direction = position - getPosition();
        float distanceToClick = direction.length();

        // 设置最小移动距离
        const float minDistance = 10.0f;
        if (distanceToClick < minDistance) {
            direction = Vec2(minDistance, 0);
            distanceToClick = minDistance;
        }

        // 在攻击范围内
        if (distanceToClick <= 1000.0f) {
            float angle = std::atan2(direction.y, direction.x) * (180.0f / M_PI);
            bullet->setRotation(-angle);
            // 计算速度
            float speed = 100.0f * (log10(bullet->getLevel())+3);
            float time = distanceToClick / speed;

            // 移动子弹到点击位置
            auto moveAction = MoveTo::create(time, position);
            auto removeAction = RemoveSelf::create();
            bullet->runAction(Sequence::create(moveAction, removeAction, nullptr));
            // 添加子弹到主场景
            this->getParent()->addChild(bullet);
            CCLOG("Angle: %f", angle);
        }
        CCLOG("Hero position: %f, %f", getPosition().x, getPosition().y);
        CCLOG("Click position: %f, %f", position.x, position.y);

    }
}

void Hero::attackWithBayonet()
{
    if (!m_bayonet)
    {
        // 创建Bayonet实例
        m_bayonet = Bayonet::create("Weapon/bayonet.png");
        if (m_bayonet)
        {
            m_bayonet->setName("bayonet");
            m_bayonet->setAnchorPoint(Vec2(-0.1f, 0.5f));
            Vec2 handpos = getPosition();
            handpos.y += 10;
            m_bayonet->setPosition(handpos);
            getParent()->addChild(m_bayonet);
        }
    }
    else
    {
        
        m_bayonet->attack();
    }
}

// 获取性别
bool Hero::getGender() {
    return m_ismale;
}

// 设置性别
void Hero::setGender(bool is_male) {
    m_ismale = is_male;
}


//获取经验值
void Hero::addExp(int exp)
{
    m_exp += exp;
    while (m_exp >= m_expToLevelUp) {
        // 玩家升级
        LevelUp();
        // 更新升级所需的经验值
        m_expToLevelUp = calculateExpToLevelUp();
    }
}

//计算升级所需要的经验值
int Hero::calculateExpToLevelUp()const
{
    return 50 * pow(1.2, m_level); // 经验需求指数增长
}

//角色升级的逻辑
void Hero::LevelUp()
{
    if (m_level <= MaxLevel) {
        setLevel(getLevel() + 1);
        //增加自身的属性值，与装备分开计算
        //后续需要更改
        setMaxHealth(getMaxHealth() + 10);
        // 升级时自动恢复生命值
        setHealth(getMaxHealth());
        setAttackPower(getAttackPower() + 2);
        m_heroism += 100;
    }
    else   //达到等级上限
        m_exp = 999999999;
}

bool Hero::isAlive()const 
{
    return m_isAlive;
}

bool Hero::checkCollision(Hero* otherHero) {
    // 获取角色的边界框
    auto thisBoundingBox = getBoundingBox();
    auto otherBoundingBox = otherHero->getBoundingBox();
    return thisBoundingBox.intersectsRect(otherBoundingBox);
}

void Hero::update(float dt) {

    if(m_bayonet)
        m_bayonet->update(dt);

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
        CCLOG("Hero update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
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
                CCLOG("Hero moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
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
                CCLOG("Hero adjusted position due to collision: (%.2f, %.2f)", adjustedPosition.x, adjustedPosition.y);
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
        CCLOG("Hero update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
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
                CCLOG("Hero moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
            }
            else {
                // 如果碰撞发生，停止角色移动（不进行反向调整）
                CCLOG("Collision detected, stopping Hero move.");
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

//获取角色元素属性
CharacterElement Hero::getElement() 
{
    return element;
}

void Hero::onMouseMove(cocos2d::EventMouse* event) {
    auto mouseEvent = static_cast<cocos2d::EventMouse*>(event);
    if (m_bayonet) {
        Vec2 mousePos = mouseEvent->getLocation();
        mousePos.y = 768 - mousePos.y;
        m_bayonet->setMousePosition(mousePos);
    }
}