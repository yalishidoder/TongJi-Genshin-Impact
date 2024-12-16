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

    // �����ӵ�ͼ����Դ
    this->setTexture("Weapon/Ammunition/bullet1.png");

    // �����ӵ�λ��
    this->setPosition(position);

    // �����ӵ��ٶȺ���������
    this->setVelocity(velocity);
    this->setLifeTime(5.0f); // �ӵ����ʱ��5��
    this->_remainingLife = this->_lifeTime;

    // ��Ӹ��µ���
    this->scheduleUpdate();

    return true;
}

void Bullet::update(float delta)
{
    // �����ӵ�λ��
    this->setPosition(this->getPosition() + _velocity * delta);

    // ����ʣ������
    _remainingLife -= delta;
    if (_remainingLife <= 0.0f)
    {
        // �ӵ������������Ƴ��ӵ�
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
    // �����ӵ��������ײ���߼�
    enemy->removeFromParentAndCleanup(true);
    this->removeFromParentAndCleanup(true);
}