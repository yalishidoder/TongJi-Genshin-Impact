#include "Bayonet.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Hero/Hero.h"


// ���캯��
Bayonet::Bayonet() : attackRange(50.0f), attackPower(100), attackCooldown(0.3f), remainingCooldown(0.0f) {
    
}

// ��������
Bayonet::~Bayonet() {
}

// ��̬��������
Bayonet* Bayonet::create(const std::string& textureFileName) {
    Bayonet* bayonet = new (std::nothrow) Bayonet();
    if (bayonet && bayonet->init(textureFileName)) {
        bayonet->autorelease();
        return bayonet;
    }
    CC_SAFE_DELETE(bayonet);
    return nullptr;
}

// ��ʼ������
bool Bayonet::init(const std::string& textureFileName) {
    if (!Sprite::initWithFile(textureFileName)) {
        return false;
    }

    // ���ػӿ�����
    m_animationCache = cocos2d::AnimationCache::getInstance();
    //m_animationCache->addAnimation(createSwingAnimation(), "bayonet_swing");
    return true;
}

// ���º���
void Bayonet::update(float delta) {
    if (remainingCooldown > 0) {
        remainingCooldown -= delta;
        isIdle = 0;
    }
    else
        isIdle = 1;
    auto parent = getParent();
    if (parent) {
        auto heros = parent->getChildren();
        for (auto hero : heros) {
            if (dynamic_cast<Hero*>(hero))
            {
                Vec2 handpos = hero->getPosition();
                handpos.y += 10;
                this->setPosition(handpos);
            }
        }
    }
    updateRotation();
    
}

void Bayonet::updateRotation() {
    if (m_mousePosition != cocos2d::Vec2::ZERO && isIdle) {
        // ����Bayonetָ�����λ�õĽǶ�
        float angle = atan2(m_mousePosition.y - getPositionY(), m_mousePosition.x - getPositionX());
        // ���Ƕ�ת��Ϊ����
        angle = CC_RADIANS_TO_DEGREES(-angle);
        // ����Bayonet����ת�Ƕ�
        m_rotationAngle = angle;
        setRotation(angle);

    }
}

// ��������
void Bayonet::attack(float angle) {
    if (remainingCooldown > 0) {
        return;
    }
    if(nextDir)
        remainingCooldown -= 0.2f;
    setRotation(m_rotationAngle + 100 * (nextDir - 0.5)+angle);
    nextDir = !nextDir;
    // ���Żӿ�����
    //playAnimation("bayonet_swing");
    checkAndHandleCollision();
    // ������ȴʱ��
    remainingCooldown = attackCooldown;
}

// ���ʹ�������˵���ײ
void Bayonet::checkAndHandleCollision() {
    auto parent = getParent();
    if (parent) {
        auto enemies = parent->getChildren();
        for (auto enemy : enemies) {
            auto temp = dynamic_cast<Enemy*>(enemy);
            if (temp && temp->isAlive())
            {
                if (this->checkCollision(temp))
                {
                    // ������ײ�߼�
                    onCollisionWithEnemy(temp);
                }
            }
        }
    }
}

// ��ײ��麯��
bool Bayonet::checkCollision(cocos2d::Node* target) {
    float distance = this->getPosition().distance(target->getPosition());
    float radius1 = this->getContentSize().width / 2;
    float radius2 = target->getContentSize().width / 2;
    return distance < (radius1 + radius2 - 2);
}

// ��������˵���ײ
void Bayonet::onCollisionWithEnemy(cocos2d::Node* enemy) {
    static_cast<Enemy*>(enemy)->takeDamage(attackPower);
}

void Bayonet::setMousePosition(const cocos2d::Vec2& position) {
    m_mousePosition = position;
}
#if 0
cocos2d::Animation* Bayonet::createSwingAnimation() {
        
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Weapon/BAYONET_SWING.plist"); 
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Weapon/BAYONET_SWING.png");

    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 1; i <= 6; ++i) {
        std::string frameName = StringUtils::format("Bayonet_swing_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // ��������������֡���Ϊ frameDelay ��
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 2.0f);
    return animation;
}

// ��Ӷ���������
void Bayonet::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
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

// ����ָ�����ƵĶ���
void Bayonet::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        Animate * m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);

    }
}
#endif