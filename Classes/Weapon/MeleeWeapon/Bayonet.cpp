#include "Bayonet.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Hero/Hero.h"


// ���캯��
Bayonet::Bayonet() : attackRange(50.0f), attackPower(10), attackCooldown(0.5f), remainingCooldown(0.0f) {
    swingAnimation = nullptr;
}

// ��������
Bayonet::~Bayonet() {
    CC_SAFE_RELEASE(swingAnimation);
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

    // ���ػӿ�������֡
    swingAnimation = cocos2d::Animation::create();
    swingAnimation->addSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName("Weapon/bayonet_swing_1.png"));
    swingAnimation->addSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName("Weapon/bayonet_swing_2.png"));
    swingAnimation->addSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName("Weapon/bayonet_swing_3.png"));
    swingAnimation->setDelayPerUnit(0.1f);
    swingAnimation->setLoops(1);
    swingAnimation->retain();

    return true;
}

// ���º���
void Bayonet::update(float delta) {
    if (remainingCooldown > 0) {
        remainingCooldown -= delta;
    }

    checkAndHandleCollision();
}

// ��������
void Bayonet::attack() {
    if (remainingCooldown > 0) {
        return;
    }

    // ���Żӿ�����
    runAction(cocos2d::Animate::create(swingAnimation));

    // ������ȴʱ��
    remainingCooldown = attackCooldown;
}

// ���ʹ�������˵���ײ
void Bayonet::checkAndHandleCollision() {
    auto parent = getParent();
    if (parent) {
        auto enemies = parent->getChildren();
        for (auto enemy : enemies) {
            if (dynamic_cast<Enemy*>(enemy))
            {
                if (this->checkCollision(enemy))
                {
                    // ������ײ�߼�
                    onCollisionWithEnemy(enemy);
                }
            }
        }
    }
}

// ��ײ��麯��
bool Bayonet::checkCollision(cocos2d::Node* target) {
    cocos2d::Rect bayonetRect = getBoundingBox();
    cocos2d::Rect targetRect = target->getBoundingBox();
    return bayonetRect.intersectsRect(targetRect);
}

// ��������˵���ײ
void Bayonet::onCollisionWithEnemy(cocos2d::Node* enemy) {
    static_cast<Enemy*>(enemy)->takeDamage(attackPower);
}