/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: ���˹���ʵ��
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#include "Hero.h"
#include "Scene/MainScene.h"
#include "Scene/OtherScene.h"

Character::Character()
    : m_health(100)         // ��ʼ����ֵ
    , m_sleepiness(100)     // ��ʼ˯��ֵ
    , m_attackPower(10)     // ��ʼ������
    , m_heroism(1000)       // ��ʼӢ�۶�
    , m_speed(200.0f)       // ��ʼ�ٶ�
    , m_ismale(0)       // ��ʼ�Ա�
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
    // ���ý�ɫ�ĳ�ʼλ��
    //setPosition(initPosition);
    
    // ���ؽ�ɫ�ĳ�ʼ����
    
    if (!m_ismale) {
        setTexture("Character/Hero/Animation/female/WALK_UP/WALK_UP_1.png");
    }
    else {
        setTexture("Character/Hero/Animation/male/male_sheet/WALK_UP_1.png");
    }
    setScale(0.8f); // ���Ը�����Ҫ������ɫ�����ű�����Ĭ������Ϊ 1.0

    // ��ʼ����ɫ�Ķ�������
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_hero");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_hero");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_hero");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_hero");

    return true;
}


// �ƶ���ָ��λ��
void Character::moveTo(const cocos2d::Vec2& targetPosition) {
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // �ƶ�ʱ�䣬�ɸ���ʵ���������
        targetPosition);
    runAction(moveAction);
}

// ����ƫ�����ƶ�
void Character::moveBy(const cocos2d::Vec2& offset) {
    auto moveAction = cocos2d::MoveBy::create(
        0.01f,  // �ƶ�ʱ�䣬�ɵ���
        offset);
    runAction(moveAction);
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// �����������߶���
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
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// ����ָ�����ƵĶ���
void Character::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);
        
    }
}

// ֹͣ��ǰ����
void Character::stopAnimation() {
    //stopAllActionsByTag(100);  // ������100��Ϊ�������action�ı�ǩ�����Զ���
    stopAllActions();
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// ��Ӷ���������
void Character::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
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



// ��������ֵ
void Character::setHealth(int health) {
    m_health = health;
    if (m_health <= 0) {
        m_isAlive = false;
    }
}

// ��ȡ����ֵ
int Character::getHealth() {
    return m_health;
}

// ��ȡ�Ա�
bool Character::getGender() {
    return m_ismale;
}

// �����Ա�
void Character::setGender(bool is_male) {
    m_ismale = is_male;
}

// ���ù�����
void Character::setAttackPower(int attackPower) {
    m_attackPower = attackPower;
}

// ��ȡ������
int Character::getAttackPower() {
    return m_attackPower;
}

bool Character::isAlive() {
    return m_isAlive;
}

bool Character::checkCollision(Character* otherCharacter) {
    // ��ȡ��ɫ�ı߽��
    auto thisBoundingBox = getBoundingBox();
    auto otherBoundingBox = otherCharacter->getBoundingBox();
    return thisBoundingBox.intersectsRect(otherBoundingBox);
}

void Character::update(float dt) {
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
        CCLOG("Character update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
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
                CCLOG("Character moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
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
                CCLOG("Character adjusted position due to collision: (%.2f, %.2f)", adjustedPosition.x, adjustedPosition.y);
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
        CCLOG("Character update: Current Position = (%.2f, %.2f), Target Position = (%.2f, %.2f)",
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
                CCLOG("Character moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
            }
            else {
                // �����ײ������ֹͣ��ɫ�ƶ��������з��������
                CCLOG("Collision detected, stopping character move.");
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

