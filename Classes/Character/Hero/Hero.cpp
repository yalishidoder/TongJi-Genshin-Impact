/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: ���˹���ʵ��
 * Author:        kfx wsm
 * Update Date:   2024.12.19
 ****************************************************************/

#include <cmath>
#include "Weapon/RangedWeapon/Ammunition/Bullet.h"
#include "Scene/OtherScene.h"
#include "AudioEngine.h"

Hero::Hero()
    : m_sleepiness(100)     // ��ʼ˯��ֵ
    , m_heroism(1000)       // ��ʼӢ�۶�
    , m_speed(200.0f)       // ��ʼ�ٶ�
    , m_ismale(0)       // ��ʼ�Ա�
    , MaxLevel(100)         // �ȼ�����
    , m_exp(0)              // ��ʼ����ֵ
    , m_expToLevelUp(100)   // ��ʼ��������Ҫ�ľ���
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
    // ���ؽ�ɫ�ĳ�ʼ����
    if (!this->getGender()) {
        this->setTexture("Character/Hero/Animation/female/female_default.png");
    }
    else {
        this->setTexture("Character/Hero/Animation/male/male_default.png");
    }
    setScale(0.8f); // ������ɫ�����ű�����Ĭ������Ϊ 1.0

    spawnPoint = initPosition;

    // ��ʼ����ɫ�Ķ�������
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");
    m_animationCache->addAnimation(createDeathAnimation(), "death_hero");
    // ������ǩ
    levelupLabel = Label::createWithSystemFont("Level UP!", "Arial", 24);
    cocos2d::Vec2 labelPos = this->getAnchorPoint();
    if (levelupLabel)
    {
        labelPos.x += 30;
        labelPos.y += 72;
        levelupLabel->setPosition(labelPos); // ���ú��ʵ�λ��
        levelupLabel->setVisible(false); // ��ʼ���ɼ�
        this->addChild(levelupLabel);
    }
    
    // ����������ǩ
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
        recoverLabel->setPosition(labelPos); // ���ú��ʵ�λ��
        recoverLabel->setVisible(false); // ��ʼ���ɼ�
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

// �ƶ���ָ��λ��
void Hero::moveTo(const cocos2d::Vec2& targetPosition) {
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // �ƶ�ʱ�䣬�ɸ���ʵ���������
        targetPosition);
    runAction(moveAction);
}

// ����ƫ�����ƶ�
void Hero::moveBy(const cocos2d::Vec2& offset) {
    auto moveAction = cocos2d::MoveBy::create(
        0.01f,  // �ƶ�ʱ�䣬�ɵ���
        offset);
    runAction(moveAction);
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// ������������
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// ����ָ�����ƵĶ���
void Hero::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);
        
    }
}

// ֹͣ��ǰ����
void Hero::stopAnimation() {
    //stopAllActionsByTag(100);  // ������100��Ϊ�������action�ı�ǩ�����Զ���
    stopAllActions();
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
    
}

// ��Ӷ���������
void Hero::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
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

void Hero::attack()
{
    //Ҫ������ģ��ҹ�
}

// ���ñ���
void Hero::setInventory(Inventory* inventory)
{
    m_inventory = inventory;
}

// �����ӿ�
Inventory* Hero::getInventory() const
{
    return m_inventory;
}

// װ������
void Hero::equipWeapon(Weapon* weapon)
{
    if (m_equippedWeapon != weapon) {
        m_equippedWeapon = weapon;
        // ������Ҫ��ȡ�����Ĺ�����
        if (weapon) {
            //m_attackPower += weapon->getAttackPower();
        }
        else {
            //m_attackPower -= m_equippedWeapon->getAttackPower();
        }
    }
}

// ��ǰ�����Ľӿ�
Weapon* Hero::getEquippedWeapon() const
{
    return m_equippedWeapon;
}

void Hero::attackWithBullet(const Vec2& position)
{
    // �����ӵ�
    auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO,this->getLevel());
    if (bullet) {
        bullet->setOwner(this);

        // �����ӵ�ͼ����Դ
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

        // ����ê��
        bullet->setAnchorPoint(Vec2(0.5f, 0.5f));

        // �����ӵ���λ��ΪӢ�۵�λ��
        bullet->setPosition(getPosition());
        // �����ӵ����ƶ�����
        Vec2 direction = position - getPosition();
        float distanceToClick = direction.length();

        // ������С�ƶ�����
        const float minDistance = 10.0f;
        if (distanceToClick < minDistance) {
            direction = Vec2(minDistance, 0);
            distanceToClick = minDistance;
        }

        // �ڹ�����Χ��
        if (distanceToClick <= 1000.0f) {
            float angle = std::atan2(direction.y, direction.x) * (180.0f / M_PI);
            bullet->setRotation(-angle);
            // �����ٶ�
            float speed = 100.0f * (log10(bullet->getLevel())+3);
            float time = distanceToClick / speed;

            // �ƶ��ӵ������λ��
            auto moveAction = MoveTo::create(time, position);
            auto removeAction = RemoveSelf::create();
            bullet->runAction(Sequence::create(moveAction, removeAction, nullptr));
            // ����ӵ���������
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
       
        // ����Bayonetʵ��
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

// ��ȡ�Ա�
bool Hero::getGender() {
    return m_ismale;
}

// �����Ա�
void Hero::setGender(bool is_male) {
    m_ismale = is_male;
}


//��ȡ����ֵ
void Hero::addExp(int exp)
{
    m_exp += exp;
    while (m_exp >= m_expToLevelUp) {
        // �������
        Upgrading = true;
        LevelUp();
        updateLevelUpLabel();
        // ������������ľ���ֵ
        m_expToLevelUp = calculateExpToLevelUp();
    }
}

//������������Ҫ�ľ���ֵ
int Hero::calculateExpToLevelUp()const
{
    return 50 * pow(1.2, m_level); // ��������ָ������
}

//��ɫ�������߼�
void Hero::LevelUp()
{
    if (m_level <= MaxLevel) {
        setLevel(getLevel() + 1);
        //�������������ֵ����װ���ֿ�����
        //������Ҫ����
        setMaxHealth(getMaxHealth() + 10);
        // ����ʱ�Զ��ָ�����ֵ
        setHealth(getMaxHealth());
        setAttackPower(getAttackPower() + 2);
        m_heroism += 100;
    }
    else   //�ﵽ�ȼ�����
        m_exp = 999999999;
}

// ����������ʾ
void Hero::updateLevelUpLabel()
{
    if (!levelupLabel) {
        CCLOG("levelupLabel is nullptr");
        return;
    }

    if (Upgrading) {
        levelupLabel->setString("Level UP!");
        levelupLabel->setVisible(true);
        // ��ʼ��ɫ
        cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
        levelupLabel->setTextColor(initialColor);

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
        cocos2d::Color3B targetColor = cocos2d::Color3B::WHITE;

        // ��ɫ���䶯��
        cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

        // �������ض���
        cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
            blinkAction,
            colorAction,
            cocos2d::CallFunc::create([=]() {
                levelupLabel->setVisible(false);
                Upgrading = false;
                }),
            nullptr
        );

        // ���ж���
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

    switch (enemy->getElement())//Ԫ�ظ����˺�
    {
        case(CharacterElement::FIRE):
            CCLOG("BURN!");
            //��Ч��
            switch (this->getElement())
            {
                //�ڻ�
                case(CharacterElement::ICE):
                    damage *= 2.0f;
                    ERType = "Melt!";
                    break;
                    //����
                case(CharacterElement::WATER):
                    damage *= 1.5f;
                    ERType = "Evaporate!";
                    break;
                    //�ֿ�
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
            //��Ч��
            switch (this->getElement())
            {
                //�ڻ�
                case(CharacterElement::FIRE):
                    damage *= 1.5f;
                    ERType = "Melt!";
                    break;
                    //����
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
            //ˮЧ��
            switch (this->getElement())
            {
                //����
                case(CharacterElement::FIRE):
                    damage *= 2.0f;
                    ERType = "Evaporate!";
                    break;
                    //����
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
            //��Ч��
            // ���
            if (isControlled && freezeSprite->isVisible())
                damage *= 3;
            break;
        default:
            break;
    }

    if (this->getElement() == enemy->getElement())
        damage = 0;

    // ������ҵ�����ֵ
    float health = this->getHealth() - damage;

    // �����˺�������ʾ
    updateDamageLabel(damage);
    // ����Ԫ�ط�Ӧ��ʾ
    updateERLabel();
    this->setHealth(health);
    // �������Ƿ�����    
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
    // ����������Ч
    if (getGender())
        cocos2d::experimental::AudioEngine::play2d("Audio/hero_male_death.mp3", false, 0.5f);
    else
        cocos2d::experimental::AudioEngine::play2d("Audio/hero_female_death.mp3", false, 0.5f);
    // ������������
    playAnimation("death_hero");

    // ���ű�������
    int audioId = cocos2d::experimental::AudioEngine::play2d("Audio/RIP.mp3", false, 0.5f);

    // ����һ��ȫ���ĺ�ɫ����
    cocos2d::LayerColor* fadeLayer = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 0));
    fadeLayer->setName("fadeLayer");
    cocos2d::Director::getInstance()->getRunningScene()->addChild(fadeLayer, 100);

    // ������Ϸ����ͼ��
    cocos2d::Sprite* gameoverSprite = cocos2d::Sprite::create("Character/hero/game_over.png");
    gameoverSprite->setName("gameoverSprite");
    gameoverSprite->setPosition(Vec2(500, 500));
    gameoverSprite->setScale(2.0f);
    gameoverSprite->setOpacity(0);
    cocos2d::Director::getInstance()->getRunningScene()->addChild(gameoverSprite, 100);

    // ����������ť
    auto respawnButton = ui::Button::create("Character/panel/Restart_icon.png");
    respawnButton->setPosition(Vec2(250, 100));
    respawnButton->setName("respawnButton");
    respawnButton->setOpacity(0);
    respawnButton->addClickEventListener([=](cocos2d::Ref* sender) {
        this->respawnHero(audioId);
        });
    cocos2d::Director::getInstance()->getRunningScene()->addChild(respawnButton, 101);

    // ����������Ϸ��ť
    auto endButton = ui::Button::create("Character/panel/Close_Icon.png");
    endButton->setScale(1.5f);
    endButton->setPosition(Vec2(750, 100));
    endButton->setName("endButton");
    endButton->setOpacity(0);
    endButton->addClickEventListener([=](cocos2d::Ref* sender) {
        this->endGame();
        });
    cocos2d::Director::getInstance()->getRunningScene()->addChild(endButton, 101);

    // ����һ�������Ķ�������1
    cocos2d::FadeTo* fadeToAction1 = cocos2d::FadeTo::create(3.0f, 244); // 3���ڽ�͸���ȴ�0����Ϊ244
    // ���ж���
    fadeLayer->runAction(fadeToAction1);

    // ����һ�������Ķ�������2
    cocos2d::FadeTo* fadeToAction2 = cocos2d::FadeTo::create(6.0f, 244); // 6���ڽ�͸���ȴ�0����Ϊ244
    gameoverSprite->runAction(fadeToAction2);

    // ����һ�������Ķ�������3
    cocos2d::FadeTo* fadeToAction3 = cocos2d::FadeTo::create(6.0f, 244); // 6���ڽ�͸���ȴ�0����Ϊ244
    respawnButton->runAction(fadeToAction3);

    // ����һ�������Ķ�������4
    cocos2d::FadeTo* fadeToAction4 = cocos2d::FadeTo::create(6.0f, 244); // 6���ڽ�͸���ȴ�0����Ϊ244
    endButton->runAction(fadeToAction4);
}

void Hero::respawnHero(int audioId)
{
    // ɾȥ��������
    /*auto respawnButton = cocos2d::Director::getInstance()->getRunningScene()->getChildByName("respawnButton");
    if (respawnButton)
        respawnButton->setVisible(false);*/
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("respawnButton");
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("endButton");
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("gameoverSprite");
    cocos2d::Director::getInstance()->getRunningScene()->removeChildByName("fadeLayer");
    cocos2d::experimental::AudioEngine::stop(audioId);


    // �������λ�õ�������
    setPosition(spawnPoint);

    // �ָ�����ֵ
    setHealth(m_full_health);

    // �ָ���������
    m_isAlive = true;
    isControlled = false;

    m_moveDirection = cocos2d::Vec2::ZERO;

    // ��ʾ���
    setVisible(true);

    // ֹͣ��������
    //stopAllActions();
    if (m_currentAnimate && m_animationCache->getAnimation("death_hero") == m_currentAnimate->getAnimation()) {
        stopAnimation();
    }
    // �ָ�������װ��
    if (m_bayonet) {
        m_bayonet->setVisible(m_isBayonetChosen);
    }
    weaponLabel->setVisible(true);

    // ����������Ч
    //cocos2d::experimental::AudioEngine::play2d("Audio/respawn.mp3", false, 0.5f);

    // �������������ʾ
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
    // ��ʱ�˳�
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

// ����Ԫ�ط�Ӧ��ʾ
void Hero::updateERLabel()
{
    ERLabel->setString(ERType);
    ERLabel->setVisible(true);
    // ��ʼ��ɫ
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    ERLabel->setTextColor(initialColor);

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
    cocos2d::Color3B targetColor = cocos2d::Color3B::WHITE;

    if (ERType == "Melt!")
        targetColor = cocos2d::Color3B::RED;
    else if (ERType == "Freeze!")
        targetColor = cocos2d::Color3B::BLUE;
    else if (ERType == "Resist!")
        targetColor = cocos2d::Color3B::GRAY;

    // ��ɫ���䶯��
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // �������ض���
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            ERLabel->setVisible(false);
            }),
        nullptr
    );

    // ���ж���
    ERLabel->runAction(hideAction);
}

// ���ñ���Ч��
void Hero::Freeze()
{
    applyControl(FreezeTime);
    cocos2d::Vec2 labelPos = this->getAnchorPoint();
    labelPos.x += 30;
    labelPos.y += 30;
    freezeSprite->setPosition(labelPos);
    freezeSprite->setVisible(isControlled);

    // ʹ��scheduleOnce����һ���ӳ�����
    this->scheduleOnce(schedule_selector(Enemy::hideFreezeSprite), FreezeTime);

    /*this->scheduleOnce([this](float dt) {
        freezeSprite->setVisible(false);
        }, FreezeTime);*/
}

// ��ӻص�����
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
    // ��ʼ��ɫ
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    recoverLabel->setTextColor(initialColor);

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
    cocos2d::Color3B targetColor = cocos2d::Color3B::GREEN;

    // ��ɫ���䶯��
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // �������ض���
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            recoverLabel->setVisible(false);
            Upgrading = false;
            }),
        nullptr
    );

    // ���ж���
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
    // ��ȡ��ɫ�ı߽��
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

    // �ָ�����ֵ
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
        // �����������߶���������Ϊѭ������
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_up_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_up_hero");
        }
    }
    else {
        // ֹͣ�������߶���
        if (m_currentAnimate && m_animationCache->getAnimation("walk_up_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    if (m_moveDown) {
        m_moveDirection.y -= m_speed * dt;
        // �����������߶���������Ϊѭ������
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_down_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_down_hero");
        }
    }
    else {
        // ֹͣ�������߶���
        if (m_currentAnimate && m_animationCache->getAnimation("walk_down_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    if (m_moveLeft) {
        m_moveDirection.x -= m_speed * dt;
        // �����������߶���������Ϊѭ������
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_left_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_left_hero");
        }
    }
    else {
        // ֹͣ�������߶���
        if (m_currentAnimate && m_animationCache->getAnimation("walk_left_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    if (m_moveRight) {
        m_moveDirection.x += m_speed * dt;
        // �����������߶���������Ϊѭ������
        if (!m_currentAnimate || m_animationCache->getAnimation("walk_right_hero") != m_currentAnimate->getAnimation()) {

            playAnimation("walk_right_hero");
        }
    }
    else {
        // ֹͣ�������߶���
        if (m_currentAnimate && m_animationCache->getAnimation("walk_right_hero") == m_currentAnimate->getAnimation()) {
            stopAnimation();
        }
    }


    //���˰�
#if 0
    // ������ƶ��������ȼ���Ŀ��λ��
    if (m_moveDirection != cocos2d::Vec2::ZERO) {
        // ����Ŀ��λ��
        cocos2d::Vec2 targetPosition = getPosition() + m_moveDirection;
        cocos2d::Vec2 beforePosition = getPosition();
        // ���������־����ɫ�ĵ�ǰ���ꡢĿ������
        CCLOG("Hero update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
            getPosition().x, getPosition().y, targetPosition.x, targetPosition.y);

        // ��ȡ��ǰ����
        cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
        if (currentScene) {
            // ���Ŀ��λ���Ƿ�����ǽ��
            bool collision = false;
            // �жϳ������ͣ������ò�ͬ����ײ��ⷽ��
            if (dynamic_cast<MainScene*>(currentScene)) {
                // ����� MainScene��ʹ�� MainScene ����ײ���
                MainScene* scene = dynamic_cast<MainScene*>(currentScene);
                collision = scene->checkCollision(targetPosition);
            }
            else if (dynamic_cast<OtherScene*>(currentScene)) {
                // ����� OtherScene��ʹ�� OtherScene ����ײ���
                OtherScene* scene = dynamic_cast<OtherScene*>(currentScene);
                collision = scene->checkCollision(targetPosition);
            }
            CCLOG("Collision check: Target Position = (%.2f, %.2f), Collision = %s",
                targetPosition.x, targetPosition.y, collision ? "True" : "False");

            if (!collision) {
                // ���û������ǽ�ڣ�ִ���ƶ�
                moveBy(m_moveDirection);
                CCLOG("Hero moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
            }
            else {
                // �������ǽ�ڣ����˻�һ�㣬�����ɫ��ס
                cocos2d::Vec2 adjustedPosition = getPosition();
                const float ajt = 1;//�˻صĶ���
                if (m_moveUp && collision) adjustedPosition.y -= ajt * m_speed * dt;
                if (m_moveDown && collision) adjustedPosition.y += ajt * m_speed * dt;
                if (m_moveLeft && collision) adjustedPosition.x += ajt * m_speed * dt;
                if (m_moveRight && collision) adjustedPosition.x -= ajt * m_speed * dt;

                // ����˻غ����λ��
                CCLOG("Hero adjusted position due to collision: (%.2f, %.2f)", adjustedPosition.x, adjustedPosition.y);
                setPosition(adjustedPosition);
            }
        }
        else {
            CCLOG("MainScene not found");
        }
    }
#endif

    //�����˰棬�������ɫһ֡�ƶ����࣬��ᳬ�����࣬�������￨ס���ѽ����
#if 1
    // ������ƶ��������ȼ���Ŀ��λ��
    if (m_moveDirection != cocos2d::Vec2::ZERO) {
        // ����Ŀ��λ��
        cocos2d::Vec2 targetPosition = getPosition() + m_moveDirection;

        // ���������־����ɫ�ĵ�ǰ���ꡢĿ������
        CCLOG("Hero update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
            getPosition().x, getPosition().y, targetPosition.x, targetPosition.y);

        // ��ȡ��ǰ���еĳ���
        cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
        if (currentScene) {
            // ���Ŀ��λ���Ƿ�����ǽ��
            bool collision = false;

            // �жϳ������Ͳ����ò�ͬ����ײ��ⷽ��
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

            // ���û����ײ��ִ���ƶ�
            if (!collision) {
                moveBy(m_moveDirection);
                CCLOG("Hero moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
            }
            else {
                // �����ײ������ֹͣ��ɫ�ƶ��������з��������
                CCLOG("Collision detected, stopping Hero move.");
                // ���ı��ɫ��λ�ã�����ͣ����ԭ��
                setPosition(getPosition());
            }
        }
        else {
            CCLOG("MainScene not found");
        }
    }
#endif

}

//��ȡ��ɫԪ������
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
            // ����Bayonetʵ��
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

    // λ�Ƽ��ܵľ���Ϊ 30 ����
    int moveDistance = 30;
    Vec2 targetDir = Vec2(moveDistance * (m_moveRight - m_moveLeft), moveDistance * (m_moveUp - m_moveDown));
    Vec2 posNow = getPosition();
    Vec2 endPosition = posNow + targetDir;
    if (endPosition.x <= moveDistance || endPosition.y <= moveDistance || endPosition.x >= visibleSize.width - moveDistance || endPosition.y >= visibleSize.height - moveDistance)
        return;
    bool collision = false;

    // ��ɢ��·�����ֳ� 100 ��
    int numSegments = 100;
    cocos2d::Vec2 segmentStep = targetDir / numSegments * 1.5;
    bool collisionDetected = false;

    // ���·���ϵ���ײ
    for (int i = 0; i < numSegments; ++i)
    {
        posNow += segmentStep;
        // ����Ƿ�ᷢ����ײ
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

    // ���û�з�����ײ��ִ��λ��
    if (!collisionDetected)
    {
        // ʹ�� MoveBy ��������ɫ�ƶ���Ŀ��λ��
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
        // ���ε�Ļ�İ뾶
        float radius = 1.0f;
        // ��Ļ����
        int numBullets = 30;
        // ÿ���ӵ�֮��ĽǶȲ�
        float angleStep = 360.0f / numBullets;
        // ����
        int numWaves = 3;
        // ��֮����ӳ�ʱ��
        float waveDelay = 0.1f;

        // ������������
        cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

        for (int wave = 0; wave < numWaves; ++wave)
        {
            // �ӳٶ���
            actions.pushBack(cocos2d::DelayTime::create(wave * waveDelay));

            // ����һ����Ļ�Ķ���
            actions.pushBack(cocos2d::CallFunc::create([=]() {
                for (int i = 0; i < numBullets; ++i)
                {
                    // �����ӵ��ĽǶ�
                    float angle = i * angleStep;
                    // ���Ƕ�ת��Ϊ����
                    float radian = angle * (M_PI / 180.0f);

                    // �����ӵ���λ��
                    cocos2d::Vec2 bulletPosition = getPosition() + cocos2d::Vec2(radius * cos(radian), radius * sin(radian));

                    // �����ӵ�
                    auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO, (m_level + 1) / 2);


                    if (bullet)
                    {
                        bullet->setOwner(this);
                        // Ϊ�ӵ���������
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


                        // �����ӵ���ê��
                        bullet->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));


                        // �����ӵ���λ��
                        bullet->setPosition(bulletPosition);
                        bullet->setRotation(-angle);

                        // �����ӵ��ķ���ʹ������Բ�ܵ����߷������
                        cocos2d::Vec2 direction(cos(radian), sin(radian));


                        // �ӵ����ٶ�
                        float speed = 300.0f;


                        // �����ӵ����ƶ�ʱ��
                        float time = 1.0f;


                        // �����ƶ�����
                        auto moveAction = cocos2d::MoveBy::create(time, direction * speed * time);


                        // �����Ƴ����������ӵ��ƶ�һ��ʱ������Ƴ�
                        auto removeAction = cocos2d::RemoveSelf::create();


                        // ���ж�������
                        bullet->runAction(cocos2d::Sequence::create(moveAction, removeAction, nullptr));


                        // ����ӵ������ڵ�
                        this->getParent()->addChild(bullet);
                    }
                }
                }));
        }


        // ���ж�������
        runAction(cocos2d::Sequence::create(actions));
    }
    else
    {
        m_skillXCoolDownTime = 3.0f;
        m_isSkillXOnCoolDown = true;
        // �ɵ�(ʵ��Ϊ�ӵ�)�İ뾶
        float radius = 1.0f;
        // ��Ļ������
        int numBullets = 20;
        // ÿ���ɵ�֮��ĽǶȲ�
        float angleStep = 360.0f / numBullets;
        // ��������
        cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;
        // ����һ����Ļ�Ķ���  
        actions.pushBack(cocos2d::CallFunc::create([=]() {
            for (int i = 0; i < numBullets; ++i)
            {
                // ����ɵ��ĽǶ� 
                float angle = i * angleStep;
                // ���Ƕ�ת��Ϊ����
                float radian = angle * (M_PI / 180.0f);
                // ����ɵ���λ��
                cocos2d::Vec2 bulletPosition = getPosition() + cocos2d::Vec2(radius * cos(radian), radius * sin(radian));
                // �����ӵ�
                auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO, m_level);
                if (bullet)
                {
                    bullet->setOwner(this);
                    bullet->setTexture("Weapon/bayonet.png");
                    // ���÷ɵ���ê��  
                    bullet->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
                    // ���÷ɵ���λ��            
                    bullet->setPosition(bulletPosition);
                    bullet->setRotation(-angle);
                    // ����ɵ��ķ���ʹ������Բ�ܵ����߷������ 
                    cocos2d::Vec2 direction(cos(radian), sin(radian));
                    // �ɵ����ٶ�
                    float speed = 200.0f;
                    // ����ɵ����ƶ�ʱ��
                    float time = 1.6f;
                    // �����ƶ�����
                    auto moveAction = cocos2d::MoveBy::create(time / 4, direction * speed * time / 4);
                    // �����Ƴ����������ɵ��ƶ�һ��ʱ������Ƴ�
                    auto removeAction = cocos2d::RemoveSelf::create();
                    // ������ת������ʹ�ɵ��ڷ��й����в�����ת
                    auto rotateAction = RotateBy::create(time / 4, 360.0f);
                    // ʹ�� Spawn ����ͬʱִ���ƶ�����ת����
                    auto spawnAction = Spawn::create(moveAction, rotateAction, nullptr);
                    // ���ж�������
                    bullet->runAction(cocos2d::Sequence::create(spawnAction, moveAction, moveAction, moveAction, removeAction, nullptr));
                    // ��ӷɵ������ڵ�
                    this->getParent()->addChild(bullet);
                }
            }
            }));
        // ���ж�������
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
        // ����������Ҫ����Ϣ
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

        // ����������Ҫ����Ϣ
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