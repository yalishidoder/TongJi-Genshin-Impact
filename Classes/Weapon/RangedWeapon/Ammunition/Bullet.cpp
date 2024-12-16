#include "Bullet.h"

Bullet::Bullet()
{
    _velocity = cocos2d::Vec2::ZERO;
    _lifeTime = 0.0f;
    _remainingLife = 0.0f;
}

Bullet::~Bullet()
{
}

Bullet* Bullet::create(const cocos2d::Vec2& position, const cocos2d::Vec2& velocity)
{
    Bullet* bullet = new Bullet();
    if (bullet && bullet->init(position, velocity))
    {
        bullet->autorelease();
        return bullet;
    }
    else
    {
        delete bullet;
        bullet = nullptr;
        return nullptr;
    }
}

bool Bullet::init(const cocos2d::Vec2& position, const cocos2d::Vec2& velocity)
{
    if (!Sprite::init())
    {
        return false;
    }

    // 加载子弹图形资源
    this->setTexture("Weapon/Ammunition/bullet1.png");

    // 设置子弹位置
    this->setPosition(position);

    // 设置子弹速度和生命周期
    this->setVelocity(velocity);
    this->setLifeTime(5.0f); // 子弹存活时间5秒
    this->_remainingLife = this->_lifeTime;

    // 添加更新调度
    this->scheduleUpdate();

    return true;
}

void Bullet::update(float delta)
{
    // 更新子弹位置
    this->setPosition(this->getPosition() + _velocity * delta);

    // 更新剩余寿命
    _remainingLife -= delta;
    if (_remainingLife <= 0.0f)
    {
        // 子弹寿命结束，移除子弹
        this->removeFromParentAndCleanup(true);
    }
}

void Bullet::setVelocity(const cocos2d::Vec2& velocity)
{
    _velocity = velocity;
}

cocos2d::Vec2 Bullet::getVelocity() const
{
    return _velocity;
}

void Bullet::setLifeTime(float lifeTime)
{
    _lifeTime = lifeTime;
    _remainingLife = _lifeTime;
}

float Bullet::getLifeTime() const
{
    return _lifeTime;
}

void Bullet::onCollisionWithEnemy(cocos2d::Node* enemy)
{
    // 处理子弹与敌人碰撞的逻辑
    enemy->removeFromParentAndCleanup(true);
    this->removeFromParentAndCleanup(true);
}