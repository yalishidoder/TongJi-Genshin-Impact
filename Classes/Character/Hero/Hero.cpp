/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: 主人公类实现
 * Author:        kfx wsm
 * Update Date:   2024.12.19
 ****************************************************************/

#include <cmath>
#include "Weapon/RangedWeapon/Ammunition/Bullet.h"
#include "Scene/OtherScene.h"
#include "AudioEngine.h"

Hero::Hero()
    : m_sleepiness(100)     // 初始睡意值
    , m_heroism(1000)       // 初始英雄度
    , m_speed(200.0f)       // 初始速度
    , m_ismale(0)       // 初始性别
    , MaxLevel(100)         // 等级上限
    , m_exp(0)              // 初始经验值
    , m_expToLevelUp(100)   // 初始升级所需要的经验
    , m_gold_coin(100)
    , m_isAlive(true)
    , Upgrading(false)
    , m_animationCache(cocos2d::AnimationCache::getInstance())
    , m_currentAnimate(nullptr)
    , m_moveDirection(cocos2d::Vec2::ZERO)
    , m_isBayonetGet(true)
    , m_isBulletGet(false)
    , m_isBayonetChosen(false)
    , m_isBulletChosen(false)
    , m_moveUp(false)
    , m_moveDown(false)
    , m_moveLeft(false)
    , m_moveRight(false)
    , m_bayonet(nullptr)
    , m_inventory(new Inventory())
{
}

Hero::~Hero() {
    CC_SAFE_RELEASE_NULL(m_animationCache);
    //CC_SAFE_RELEASE_NULL(m_currentAnimate);
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

    spawnPoint = initPosition;

    // 初始化角色的动画缓存
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");
    m_animationCache->addAnimation(createDeathAnimation(), "death_hero");
    // 创建标签
    levelupLabel = Label::createWithSystemFont("Level UP!", "Arial", 24);
    cocos2d::Vec2 labelPos = this->getAnchorPoint();
    if (levelupLabel)
    {
        labelPos.x += 30;
        labelPos.y += 72;
        levelupLabel->setPosition(labelPos); // 设置合适的位置
        levelupLabel->setVisible(false); // 初始不可见
        this->addChild(levelupLabel);
    }
    
    // 创建武器标签
    weaponLabel = Label::createWithSystemFont("", "Arial", 14);
    if (weaponLabel) {
        labelPos.x += 30;
        labelPos.y -= 20;;
        if (m_isBayonetChosen)
            weaponLabel->setString("Bayonet");
        if (m_isBulletChosen && m_isBulletGet)
            weaponLabel->setString("Bullet");
        weaponLabel->setPosition(labelPos); 
        weaponLabel->setVisible(true);
        this->addChild(weaponLabel);
    }
    else {
        CCLOG("Failed to create weaponLabel");
    }

    recoverLabel= Label::createWithSystemFont("Recover", "Arial", 24);
    if (recoverLabel)
    {
        recoverLabel->setPosition(labelPos); // 设置合适的位置
        recoverLabel->setVisible(false); // 初始不可见
        this->addChild(recoverLabel);
    }

    damageLabel = cocos2d::Label::createWithTTF("", "fonts/arial.ttf", 24);
    if (damageLabel)
    {
        labelPos.x -= 50;
        labelPos.y -= 10;
        damageLabel->setPosition(labelPos);
        this->addChild(damageLabel);
        damageLabel->setVisible(false);
    }

    ERLabel = cocos2d::Label::createWithTTF("", "fonts/arial.ttf", 20);
    if (ERLabel)
    {
        labelPos.x += 100;
        ERLabel->setPosition(labelPos);
        this->addChild(ERLabel);
        ERLabel->setVisible(false);
    }

    freezeSprite = cocos2d::Sprite::create("Character/ER/Crystal_Icon.png");
    if (freezeSprite)
    {
        freezeSprite->setPosition(this->getPosition());
        this->addChild(freezeSprite);
        freezeSprite->setOpacity(150);
        freezeSprite->setVisible(isControlled);
    }

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
        std::string frameName;
        if (!m_ismale)
            frameName = StringUtils::format("WALK_UP_%dF.png", i);
        else
            frameName = StringUtils::format("WALK_UP_%dM.png", i);
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
        std::string frameName;
        if (!m_ismale)
            frameName = StringUtils::format("WALK_DOWN_%dF.png", i);
        else
            frameName = StringUtils::format("WALK_DOWN_%dM.png", i);
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
        std::string frameName;
        if (!m_ismale)
            frameName = StringUtils::format("WALK_LEFT_%dF.png", i);
        else
            frameName = StringUtils::format("WALK_LEFT_%dM.png", i);
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
        std::string frameName;
        if (!m_ismale)
            frameName = StringUtils::format("WALK_RIGHT_%dF.png", i);
        else
            frameName = StringUtils::format("WALK_RIGHT_%dM.png", i);
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
cocos2d::Animation* Hero::createDeathAnimation() {
    if (!m_ismale) {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/DEATH_FEMALE.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/female/DEATH_FEMALE.png");
    }
    else {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/DEATH_MALE.plist");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Hero/Animation/male/DEATH_MALE.png");
    }
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 0; i <= 6; ++i) {
        std::string frameName;
        if (!m_ismale)
            frameName = StringUtils::format("HERO_DEATH_FEMALE-%d.png", i);
        else
            frameName = StringUtils::format("HERO_DEATH_MALE-%d.png", i);
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

// 设置背包
void Hero::setInventory(Inventory* inventory)
{
    m_inventory = inventory;
}

// 背包接口
Inventory* Hero::getInventory() const
{
    return m_inventory;
}

// 装备武器
void Hero::equipWeapon(Weapon* weapon)
{
    if (m_equippedWeapon != weapon) {
        m_equippedWeapon = weapon;
        // 这里需要获取武器的攻击力
        if (weapon) {
            //m_attackPower += weapon->getAttackPower();
        }
        else {
            //m_attackPower -= m_equippedWeapon->getAttackPower();
        }
    }
}

// 当前武器的接口
Weapon* Hero::getEquippedWeapon() const
{
    return m_equippedWeapon;
}

void Hero::attackWithBullet(const Vec2& position)
{
    // 创建子弹
    auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO,this->getLevel());
    if (bullet) {
        bullet->setOwner(this);

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

void Hero::attackWithBayonet(float angle)
{
    weaponLabel->setString("Bayonet");
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
            m_bayonet->attackPower = 10 * (m_level + 1);
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
        Upgrading = true;
        LevelUp();
        updateLevelUpLabel();
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

// 更新升级显示
void Hero::updateLevelUpLabel()
{
    if (!levelupLabel) {
        CCLOG("levelupLabel is nullptr");
        return;
    }

    if (Upgrading) {
        levelupLabel->setString("Level UP!");
        levelupLabel->setVisible(true);
        // 初始颜色
        cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
        levelupLabel->setTextColor(initialColor);

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
        cocos2d::Color3B targetColor = cocos2d::Color3B::WHITE;

        // 颜色渐变动作
        cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

        // 最终隐藏动作
        cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
            blinkAction,
            colorAction,
            cocos2d::CallFunc::create([=]() {
                levelupLabel->setVisible(false);
                Upgrading = false;
                }),
            nullptr
        );

        // 运行动作
        levelupLabel->runAction(hideAction);
    }
    CCLOG("updateLevelUpLabel called");
}

void Hero::setGoldCoin(int value)
{
    m_gold_coin = value;
}

int Hero::getGoldCoin()const
{
    return m_gold_coin;
}

void Hero::takeDamage(float damage, Enemy* enemy)
{
    if (!m_isAlive)
        return;

    switch (enemy->getElement())//元素附加伤害
    {
        case(CharacterElement::FIRE):
            CCLOG("BURN!");
            //火效果
            switch (this->getElement())
            {
                //融化
                case(CharacterElement::ICE):
                    damage *= 2.0f;
                    ERType = "Melt!";
                    break;
                    //蒸发
                case(CharacterElement::WATER):
                    damage *= 1.5f;
                    ERType = "Evaporate!";
                    break;
                    //抵抗
                case(CharacterElement::ROCK):
                    damage *= 0.8f;
                    ERType = "Resist!";
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
                //融化
                case(CharacterElement::FIRE):
                    damage *= 1.5f;
                    ERType = "Melt!";
                    break;
                    //冻结
                case(CharacterElement::WATER):
                    ERType = "Freeze!";
                    Freeze();
                    break;
                case(CharacterElement::ROCK):
                    damage *= 0.8f;
                    ERType = "Resist!";
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
                //蒸发
                case(CharacterElement::FIRE):
                    damage *= 2.0f;
                    ERType = "Evaporate!";
                    break;
                    //冻结
                case(CharacterElement::ICE):
                    ERType = "Freeze!";
                    Freeze();
                    break;
                case(CharacterElement::ROCK):
                    damage *= 0.8f;
                    ERType = "Resist!";
                    break;
                default:
                    break;
            }
            break;
        case(CharacterElement::ROCK):
            CCLOG("YOU SHALL NOT PASS!");
            //岩效果
            // 碎冰
            if (isControlled && freezeSprite->isVisible())
                damage *= 3;
            break;
        default:
            break;
    }

    if (this->getElement() == enemy->getElement())
        damage = 0;

    // 减少玩家的生命值
    float health = this->getHealth() - damage;

    // 更新伤害数字显示
    updateDamageLabel(damage);
    // 更新元素反应显示
    updateERLabel();
    this->setHealth(health);
    // 检查玩家是否死亡    
    if (health < 0) {
        setHealth(0);
        setDeath();
    }

}

void Hero::setDeath()
{
    if (!m_isAlive) 
        return;
    stopAllActions();
    remove("hero.txt");
    m_isAlive = false;
    m_isBayonetChosen = false;
    m_isBulletChosen = false;
    weaponLabel->setVisible(0);
    if (m_bayonet)
        m_bayonet->setVisible(0);
    isControlled = true;
    // 播放死亡音效
    if (getGender())
        cocos2d::experimental::AudioEngine::play2d("Audio/hero_male_death.mp3", false, 0.5f);
    else
        cocos2d::experimental::AudioEngine::play2d("Audio/hero_female_death.mp3", false, 0.5f);
    // 播放死亡动画
    playAnimation("death_hero");

    // 播放背景音乐
    int audioId = cocos2d::experimental::AudioEngine::play2d("Audio/RIP.mp3", false, 0.5f);

    // 创建一个全屏的黑色矩形
    cocos2d::LayerColor* fadeLayer = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 0));
    fadeLayer->setName("fadeLayer");
    cocos2d::Director::getInstance()->getRunningScene()->addChild(fadeLayer, 100);

    // 创建游戏结束图标
    cocos2d::Sprite* gameoverSprite = cocos2d::Sprite::create("Character/hero/game_over.png");
    gameoverSprite->setName("gameoverSprite");
    gameoverSprite->setPosition(Vec2(500, 500));
    gameoverSprite->setScale(2.0f);
    gameoverSprite->setOpacity(0);
    cocos2d::Director::getInstance()->getRunningScene()->addChild(gameoverSprite, 100);

    // 创建重生按钮
    auto respawnButton = ui::Button::create("Character/panel/Restart_icon.png");
    respawnButton->setPosition(Vec2(250, 100));
    respawnButton->setName("respawnButton");
    respawnButton->setOpacity(0);
    respawnButton->addClickEventListener([=](cocos2d::Ref* sender) {
        this->respawnHero(audioId);
        });
    cocos2d::Director::getInstance()->getRunningScene()->addChild(respawnButton, 101);

    // 创建结束游戏按钮
    auto endButton = ui::Button::create("Character/panel/Close_Icon.png");
    endButton->setScale(1.5f);
    endButton->setPosition(Vec2(750, 100));
    endButton->setName("endButton");
    endButton->setOpacity(0);
    endButton->addClickEventListener([=](cocos2d::Ref* sender) {
        this->endGame();
        });
    cocos2d::Director::getInstance()->getRunningScene()->addChild(endButton, 101);

    // 创建一个渐暗的动作序列1
    cocos2d::FadeTo* fadeToAction1 = cocos2d::FadeTo::create(3.0f, 244); // 3秒内将透明度从0渐变为244
    // 运行动作
    fadeLayer->runAction(fadeToAction1);

    // 创建一个渐暗的动作序列2
    cocos2d::FadeTo* fadeToAction2 = cocos2d::FadeTo::create(6.0f, 244); // 6秒内将透明度从0渐变为244
    gameoverSprite->runAction(fadeToAction2);

    // 创建一个渐暗的动作序列3
    cocos2d::FadeTo* fadeToAction3 = cocos2d::FadeTo::create(6.0f, 244); // 6秒内将透明度从0渐变为244
    respawnButton->runAction(fadeToAction3);

    // 创建一个渐暗的动作序列4
    cocos2d::FadeTo* fadeToAction4 = cocos2d::FadeTo::create(6.0f, 244); // 6秒内将透明度从0渐变为244
    endButton->runAction(fadeToAction4);
}

void Hero::respawnHero(int audioId)
{
    // 删去死亡界面
    /*auto respawnButton = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("respawnButton");
    if (respawnButton)
        respawnButton->setVisible(false);*/
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("respawnButton");
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("endButton");
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("gameoverSprite");
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("fadeLayer");
    cocos2d::experimental::AudioEngine::stop(audioId);


    // 重置玩家位置到出生点
    setPosition(spawnPoint);

    // 恢复生命值
    setHealth(m_full_health);

    // 恢复其他属性
    m_isAlive = true;
    isControlled = false;

    m_moveDirection = cocos2d::Vec2::ZERO;

    // 显示玩家
    setVisible(true);

    // 停止死亡动画
    //stopAllActions();
    if (m_currentAnimate && m_animationCache->getAnimation("death_hero") == m_currentAnimate->getAnimation()) {
        stopAnimation();
    }
    // 恢复武器和装备
    if (m_bayonet) {
        m_bayonet->setVisible(m_isBayonetChosen);
    }
    weaponLabel->setVisible(true);

    // 播放重生音效
    //cocos2d::experimental::AudioEngine::play2d("Audio/respawn.mp3", false, 0.5f);

    // 隐藏死亡相关显示
    if (freezeSprite) {
        freezeSprite->setVisible(false);
    }
    if (recoverLabel) {
        recoverLabel->setVisible(false);
    }
    if (damageLabel) {
        damageLabel->setVisible(false);
    }
    if (ERLabel) {
        ERLabel->setVisible(false);
    }
}

void Hero::endGame()
{
    // 延时退出
    cocos2d::DelayTime* delay = cocos2d::DelayTime::create(2.0f);
    cocos2d::CallFunc* endGame = cocos2d::CallFunc::create([]() {
        cocos2d::Director::getInstance()->end();
        });
    cocos2d::Sequence* sequence = cocos2d::Sequence::create(delay, endGame, nullptr);
    cocos2d::Director::getInstance()->getRunningScene()->runAction(sequence);
}

void Hero::updateDamageLabel(int damage)
{
    damageLabel->setString('-' + std::to_string(damage));
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

// 更新元素反应显示
void Hero::updateERLabel()
{
    ERLabel->setString(ERType);
    ERLabel->setVisible(true);
    // 初始颜色
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    ERLabel->setTextColor(initialColor);

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
    cocos2d::Color3B targetColor = cocos2d::Color3B::WHITE;

    if (ERType == "Melt!")
        targetColor = cocos2d::Color3B::RED;
    else if (ERType == "Freeze!")
        targetColor = cocos2d::Color3B::BLUE;
    else if (ERType == "Resist!")
        targetColor = cocos2d::Color3B::GRAY;

    // 颜色渐变动作
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // 最终隐藏动作
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            ERLabel->setVisible(false);
            }),
        nullptr
    );

    // 运行动作
    ERLabel->runAction(hideAction);
}

// 设置冰冻效果
void Hero::Freeze()
{
    applyControl(FreezeTime);
    cocos2d::Vec2 labelPos = this->getAnchorPoint();
    labelPos.x += 30;
    labelPos.y += 30;
    freezeSprite->setPosition(labelPos);
    freezeSprite->setVisible(isControlled);

    // 使用scheduleOnce安排一个延迟任务
    this->scheduleOnce(schedule_selector(Enemy::hideFreezeSprite), FreezeTime);

    /*this->scheduleOnce([this](float dt) {
        freezeSprite->setVisible(false);
        }, FreezeTime);*/
}

// 添加回调函数
void Hero::hideFreezeSprite(float dt)
{
    freezeSprite->setVisible(false);
}

void Hero::updateRecoverLabel()
{
    if (!recoverLabel)
        return;
    recoverLabel->setString("Recover!");
    recoverLabel->setVisible(true);
    // 初始颜色
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    recoverLabel->setTextColor(initialColor);

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
    cocos2d::Color3B targetColor = cocos2d::Color3B::GREEN;

    // 颜色渐变动作
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // 最终隐藏动作
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            recoverLabel->setVisible(false);
            Upgrading = false;
            }),
        nullptr
    );

    // 运行动作
    recoverLabel->runAction(hideAction);
}

void Hero::Recover(float delta)
{
    cocos2d::Vec2 distanceToSpawnPoint= spawnPoint- this->getPosition();
    float distance = distanceToSpawnPoint.length();

    if (distance < 10.0f)
    {
        m_health = m_full_health;
        updateRecoverLabel();
    }
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

void Hero::resetAnimationCache()
{
    if (m_animationCache)
    {
        m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
        m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
        m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
        m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");
        m_animationCache->addAnimation(createDeathAnimation(), "death_hero");
    }
}

void Hero::update(float dt) 
{

    if (isControlled|| !m_isAlive)
        return;

    // 恢复生命值
    Recover(dt);

    if(m_bayonet)
        m_bayonet->update(dt);
    if (m_isZSkillUnlock) {
        if (m_skillZCoolDownTime > 0) {
            m_skillZCoolDownTime -= dt;
        }
        else
            m_isSkillZOnCoolDown = 0;
    }
    if (m_isXSkillUnlock) {
        if (m_skillXCoolDownTime > 0) {
            m_skillXCoolDownTime -= dt;
        }
        else
            m_isSkillXOnCoolDown = 0;
    }
    if (m_isCSkillUnlock) {
        if (m_skillCCoolDownTime > 0) {
            m_skillCCoolDownTime -= dt;
        }
        else
            m_isSkillCOnCoolDown = 0;
    }

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

void Hero::ChangeToBayonet()
{
    if (!m_isBayonetGet)
        return;
    weaponLabel->setString("Bayonet");
    if (m_isBayonetChosen)
        return;
    else
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
            m_bayonet->setVisible(true);
        m_isBayonetChosen = true;
        m_isBulletChosen = false;
    }
}

void Hero::ChangeToBullet()
{
    if (!m_isBulletGet)
        return;
    if (m_isBulletChosen)
        return;
    else
    {
        weaponLabel->setString("Bullet");
        m_isBulletChosen = true;
        m_isBayonetChosen = false;
        if (m_bayonet)
            m_bayonet->setVisible(false);
    }
}

void Hero::SkillZ()
{
    if (!m_isZSkillUnlock || m_isSkillZOnCoolDown)
        return;
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    // 位移技能的距离为 30 像素
    int moveDistance = 30;
    Vec2 targetDir = Vec2(moveDistance * (m_moveRight - m_moveLeft), moveDistance * (m_moveUp - m_moveDown));
    Vec2 posNow = getPosition();
    Vec2 endPosition = posNow + targetDir;
    if (endPosition.x <= moveDistance || endPosition.y <= moveDistance || endPosition.x >= visibleSize.width - moveDistance || endPosition.y >= visibleSize.height - moveDistance)
        return;
    bool collision = false;

    // 离散化路径，分成 100 段
    int numSegments = 100;
    cocos2d::Vec2 segmentStep = targetDir / numSegments * 1.5;
    bool collisionDetected = false;

    // 检查路径上的碰撞
    for (int i = 0; i < numSegments; ++i)
    {
        posNow += segmentStep;
        // 检查是否会发生碰撞
        bool collision = false;
        if (dynamic_cast<MainScene*>(currentScene))
        {
            MainScene* scene = dynamic_cast<MainScene*>(currentScene);
            collision = scene->checkCollision(posNow);
        }
        else if (dynamic_cast<OtherScene*>(currentScene))
        {
            OtherScene* scene = dynamic_cast<OtherScene*>(currentScene);
            collision = scene->checkCollision(posNow);
        }

        if (collision)
        {
            collisionDetected = true;
            break;
        }
    }

    // 如果没有发生碰撞，执行位移
    if (!collisionDetected)
    {
        // 使用 MoveBy 动作将角色移动到目标位置
        auto moveAction = cocos2d::MoveBy::create(0.1f, targetDir);
        runAction(moveAction);
        m_skillZCoolDownTime = 1.5f;
        m_isSkillZOnCoolDown = true;
    }
}

void Hero::SkillX()
{
    if (!m_isXSkillUnlock || m_isSkillXOnCoolDown)
        return;
    if (m_isBulletChosen) {
        m_skillXCoolDownTime = 3.0f;
        m_isSkillXOnCoolDown = true;
        // 环形弹幕的半径
        float radius = 1.0f;
        // 弹幕数量
        int numBullets = 30;
        // 每个子弹之间的角度差
        float angleStep = 360.0f / numBullets;
        // 波数
        int numWaves = 3;
        // 波之间的延迟时间
        float waveDelay = 0.1f;

        // 创建动作序列
        cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

        for (int wave = 0; wave < numWaves; ++wave)
        {
            // 延迟动作
            actions.pushBack(cocos2d::DelayTime::create(wave * waveDelay));

            // 发射一波弹幕的动作
            actions.pushBack(cocos2d::CallFunc::create([=]() {
                for (int i = 0; i < numBullets; ++i)
                {
                    // 计算子弹的角度
                    float angle = i * angleStep;
                    // 将角度转换为弧度
                    float radian = angle * (M_PI / 180.0f);

                    // 计算子弹的位置
                    cocos2d::Vec2 bulletPosition = getPosition() + cocos2d::Vec2(radius * cos(radian), radius * sin(radian));

                    // 创建子弹
                    auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO, (m_level + 1) / 2);


                    if (bullet)
                    {
                        bullet->setOwner(this);
                        // 为子弹设置纹理
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


                        // 设置子弹的锚点
                        bullet->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));


                        // 设置子弹的位置
                        bullet->setPosition(bulletPosition);
                        bullet->setRotation(-angle);

                        // 计算子弹的方向，使其沿着圆周的切线方向飞行
                        cocos2d::Vec2 direction(cos(radian), sin(radian));


                        // 子弹的速度
                        float speed = 300.0f;


                        // 计算子弹的移动时间
                        float time = 1.0f;


                        // 创建移动动作
                        auto moveAction = cocos2d::MoveBy::create(time, direction * speed * time);


                        // 创建移除动作，当子弹移动一定时间后将其移除
                        auto removeAction = cocos2d::RemoveSelf::create();


                        // 运行动作序列
                        bullet->runAction(cocos2d::Sequence::create(moveAction, removeAction, nullptr));


                        // 添加子弹到父节点
                        this->getParent()->addChild(bullet);
                    }
                }
                }));
        }


        // 运行动作序列
        runAction(cocos2d::Sequence::create(actions));
    }
    else
    {
        m_skillXCoolDownTime = 3.0f;
        m_isSkillXOnCoolDown = true;
        // 飞刀(实际为子弹)的半径
        float radius = 1.0f;
        // 弹幕的数量
        int numBullets = 20;
        // 每个飞刀之间的角度差
        float angleStep = 360.0f / numBullets;
        // 动作序列
        cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;
        // 发射一波弹幕的动作  
        actions.pushBack(cocos2d::CallFunc::create([=]() {
            for (int i = 0; i < numBullets; ++i)
            {
                // 计算飞刀的角度 
                float angle = i * angleStep;
                // 将角度转换为弧度
                float radian = angle * (M_PI / 180.0f);
                // 计算飞刀的位置
                cocos2d::Vec2 bulletPosition = getPosition() + cocos2d::Vec2(radius * cos(radian), radius * sin(radian));
                // 创建子弹
                auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO, m_level);
                if (bullet)
                {
                    bullet->setOwner(this);
                    bullet->setTexture("Weapon/bayonet.png");
                    // 设置飞刀的锚点  
                    bullet->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
                    // 设置飞刀的位置            
                    bullet->setPosition(bulletPosition);
                    bullet->setRotation(-angle);
                    // 计算飞刀的方向，使其沿着圆周的切线方向飞行 
                    cocos2d::Vec2 direction(cos(radian), sin(radian));
                    // 飞刀的速度
                    float speed = 200.0f;
                    // 计算飞刀的移动时间
                    float time = 1.6f;
                    // 创建移动动作
                    auto moveAction = cocos2d::MoveBy::create(time / 4, direction * speed * time / 4);
                    // 创建移除动作，当飞刀移动一定时间后将其移除
                    auto removeAction = cocos2d::RemoveSelf::create();
                    // 创建旋转动作，使飞刀在飞行过程中不断旋转
                    auto rotateAction = RotateBy::create(time / 4, 360.0f);
                    // 使用 Spawn 动作同时执行移动和旋转动作
                    auto spawnAction = Spawn::create(moveAction, rotateAction, nullptr);
                    // 运行动作序列
                    bullet->runAction(cocos2d::Sequence::create(spawnAction, moveAction, moveAction, moveAction, removeAction, nullptr));
                    // 添加飞刀到父节点
                    this->getParent()->addChild(bullet);
                }
            }
            }));
        // 运行动作序列
        runAction(cocos2d::Sequence::create(actions));
    }

}

void Hero::SkillC()
{

}

void Hero::saveProfile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << m_sleepiness << std::endl;
        file << m_health << std::endl;
        file << m_speed << std::endl;
        file << m_ismale << std::endl;
        file << MaxLevel << std::endl;
        file << m_level << std::endl;
        file << m_exp << std::endl;
        file << m_expToLevelUp << std::endl;
        file << m_gold_coin << std::endl;
        file << m_isAlive << std::endl;
        file << Upgrading << std::endl;
        file << m_moveDirection.x << " " << m_moveDirection.y << std::endl;
        file << m_isBayonetGet << std::endl;
        file << m_isBulletGet << std::endl;
        file << m_isBayonetChosen << std::endl;
        file << m_isBulletChosen << std::endl;
        file << m_isXSkillUnlock << std::endl;
        // 保存其他需要的信息
        file.close();
        CCLOG("Profile saved successfully.");
    }
    else {
        CCLOG("Unable to open file for saving.");
    }
}

void Hero::loadProfile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> m_sleepiness;
        file >> m_health;
        file >> m_speed;
        file >> m_ismale;
        file >> MaxLevel;
        file >> m_level;
        file >> m_exp;
        file >> m_expToLevelUp;
        file >> m_gold_coin;
        file >> m_isAlive;
        file >> Upgrading;
        file >> m_moveDirection.x >> m_moveDirection.y;
        file >> m_isBayonetGet;
        file >> m_isBulletGet;
        file >> m_isBayonetChosen;
        file >> m_isBulletChosen;
        file >> m_isXSkillUnlock;

        // 加载其他需要的信息
        file.close();
        CCLOG("Profile loaded successfully.");
    }
    else {
        CCLOG("Unable to open file for loading.");
    }
    if (!getGender()) {
        setTexture("Character/Hero/Animation/female/female_default.png");
    }
    else {
        setTexture("Character/Hero/Animation/male/male_default.png");
    }
    resetAnimationCache();
}