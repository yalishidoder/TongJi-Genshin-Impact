/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: ���˹���ʵ��
 * Author:        kfx wsm
 * Update Date:   2024.12.19
 ****************************************************************/

#include <cmath>
#include "Hero.h"
#include "Character/CharacterBase.h"
#include "Weapon/RangedWeapon/Ammunition/Bullet.h"
#include "Weapon/MeleeWeapon/Bayonet.h"
#include "Scene/MainScene.h"
#include "Scene/OtherScene.h"


Hero::Hero()
    : m_sleepiness(100)     // ��ʼ˯��ֵ
    , m_heroism(1000)       // ��ʼӢ�۶�
    , m_speed(200.0f)       // ��ʼ�ٶ�
    , m_ismale(0)       // ��ʼ�Ա�
    , MaxLevel(100)         // �ȼ�����
    , m_exp(0)              // ��ʼ����ֵ
    , m_expToLevelUp(100)   // ��ʼ��������Ҫ�ľ���
    , m_isAlive(true)
    , Upgrading(false)
    , m_animationCache(cocos2d::AnimationCache::getInstance())
    , m_currentAnimate(nullptr)
    , m_moveDirection(cocos2d::Vec2::ZERO)
    , m_isBayonetGet(true)
    , m_isBulletGet(true)
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

    // ��ʼ����ɫ�Ķ�������
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");

    // ����������ǩ
    levelupLabel = Label::createWithSystemFont("Level UP!", "Arial", 24);
    if (levelupLabel) {
        cocos2d::Vec2 labelPos = this->getAnchorPoint();
        labelPos.x += 30;
        labelPos.y += 60;
        levelupLabel->setPosition(labelPos); // ���ú��ʵ�λ��
        levelupLabel->setVisible(false); // ��ʼ���ɼ�
        this->addChild(levelupLabel);
    }
    else {
        CCLOG("Failed to create levelupLabel");
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
            frameName = StringUtils::format("WALK_UP_%d.png", i);
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
            frameName = StringUtils::format("WALK_DOWN_%d.png", i);
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
            frameName = StringUtils::format("WALK_LEFT_%d.png", i);
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
            frameName = StringUtils::format("WALK_RIGHT_%d.png", i);
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
        m_animationCache->destroyInstance();
        CCLOG("Cache destroyed!");
        m_animationCache = cocos2d::AnimationCache::getInstance();
        m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
        m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
        m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
        m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");
    }
}

void Hero::update(float dt) {
#if 0
    // �����Ա��޸�ģ��
    if (!this->getGender()) {
        this->setTexture("Character/Hero/Animation/female/female_default.png");
    }
    else {
        this->setTexture("Character/Hero/Animation/male/male_default.png");
    }
#endif
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
                    auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO, 1);


                    if (bullet)
                    {
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

    }

}

void Hero::SkillC()
{

}
