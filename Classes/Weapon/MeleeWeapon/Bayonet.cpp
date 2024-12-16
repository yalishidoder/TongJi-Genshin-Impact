#include "Bayonet.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Hero/Hero.h"


// 构造函数
Bayonet::Bayonet() : attackRange(50.0f), attackPower(10), attackCooldown(0.5f), remainingCooldown(0.0f) {
    swingAnimation = nullptr;
}

// 析构函数
Bayonet::~Bayonet() {
    CC_SAFE_RELEASE(swingAnimation);
}

// 静态创建函数
Bayonet* Bayonet::create(const std::string& textureFileName) {
    Bayonet* bayonet = new (std::nothrow) Bayonet();
    if (bayonet && bayonet->init(textureFileName)) {
        bayonet->autorelease();
        return bayonet;
    }
    CC_SAFE_DELETE(bayonet);
    return nullptr;
}

// 初始化函数
bool Bayonet::init(const std::string& textureFileName) {
    if (!Sprite::initWithFile(textureFileName)) {
        return false;
    }

    // 加载挥砍动画的帧
    swingAnimation = cocos2d::Animation::create();
    swingAnimation->addSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName("Weapon/bayonet_swing_1.png"));
    swingAnimation->addSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName("Weapon/bayonet_swing_2.png"));
    swingAnimation->addSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName("Weapon/bayonet_swing_3.png"));
    swingAnimation->setDelayPerUnit(0.1f);
    swingAnimation->setLoops(1);
    swingAnimation->retain();

    return true;
}

// 更新函数
void Bayonet::update(float delta) {
    if (remainingCooldown > 0) {
        remainingCooldown -= delta;
    }

    checkAndHandleCollision();
}

// 攻击函数
void Bayonet::attack() {
    if (remainingCooldown > 0) {
        return;
    }

    // 播放挥砍动画
    runAction(cocos2d::Animate::create(swingAnimation));

    // 重置冷却时间
    remainingCooldown = attackCooldown;
}

// 检查和处理与敌人的碰撞
void Bayonet::checkAndHandleCollision() {
    auto parent = getParent();
    if (parent) {
        auto enemies = parent->getChildren();
        for (auto enemy : enemies) {
            if (dynamic_cast<Enemy*>(enemy))
            {
                if (this->checkCollision(enemy))
                {
                    // 处理碰撞逻辑
                    onCollisionWithEnemy(enemy);
                }
            }
        }
    }
}

// 碰撞检查函数
bool Bayonet::checkCollision(cocos2d::Node* target) {
    cocos2d::Rect bayonetRect = getBoundingBox();
    cocos2d::Rect targetRect = target->getBoundingBox();
    return bayonetRect.intersectsRect(targetRect);
}

// 处理与敌人的碰撞
void Bayonet::onCollisionWithEnemy(cocos2d::Node* enemy) {
    static_cast<Enemy*>(enemy)->takeDamage(attackPower);
}