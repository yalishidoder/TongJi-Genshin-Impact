#include "Bullet.h"
#include "Character/Enemy/Enemy.h"

Bullet::Bullet()
{
    _velocity = cocos2d::Vec2::ZERO;
    _lifeTime = 0.0f;
    _remainingLife = 0.0f;
    _level = 1;
}

Bullet::~Bullet()
{
}

Bullet* Bullet::create(const cocos2d::Vec2& position, const cocos2d::Vec2& velocity, const int level)
{
    Bullet* bullet = new Bullet;
    if (bullet && bullet->init(position, velocity, level))
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

bool Bullet::init(const cocos2d::Vec2& position, const cocos2d::Vec2& velocity,const int level)
{
    if (!Sprite::init())
    {
        return false;
    }
    //Ĭ���ӵ�����
    this->setTexture("Weapon/Ammunition/bullet_default.png");

    // �����ӵ�λ��
    this->setPosition(position);

    // �����ӵ��ٶȺ���������
    this->setVelocity(velocity);
    this->setLifeTime(5.0f); // �ӵ����ʱ��5��
    this->_remainingLife = this->_lifeTime;

    // �����ӵ��ȼ�
    this->setLevel(level);

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
    else
        checkAndHandleCollision();

}

void Bullet::setVelocity(const cocos2d::Vec2& velocity)
{
    _velocity = velocity;
}

cocos2d::Vec2 Bullet::getVelocity() const
{
    return _velocity;
}

void Bullet::setLevel(int level)
{
    _level = level;
}

int Bullet::getLevel() const
{
    return _level;
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

// ��ײ����봦����
void Bullet::checkAndHandleCollision()
{
    // ��ȡ��ǰ����
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    if (currentScene)
    {
        // ��ȡ�����е����е���
        auto enemies = currentScene->getChildren();
        for (auto enemy : enemies)
        {
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

// ��ײ�����߼�
void Bullet::onCollisionWithEnemy(cocos2d::Node* enemy)
{
    // ���ٵ��˵�����ֵ
    static_cast<Enemy*>(enemy)->takeDamage((this->getLevel()+1.5)*20);

    // �����ӵ�
    this->removeFromParentAndCleanup(true);
}

// ���ƺ����ײ��⺯��
#if 0
bool Bullet::checkCollision(cocos2d::Node* target) {
    // ��ȡ�ӵ���Ŀ��ı߽��
    cocos2d::Rect bulletBoundingBox = this->getBoundingBox();
    cocos2d::Rect targetBoundingBox = target->getBoundingBox();

    // ��ȷ��ײ��⣬���Ǳ߽���Ƿ��ཻ
    bool collision = bulletBoundingBox.intersectsRect(targetBoundingBox);

    if (collision) {
        return true;
    }
    return false;
}
#endif
#if 1
bool Bullet::checkCollision(cocos2d::Node* target) {
    float distance = this->getPosition().distance(target->getPosition());
    float radius1 = this->getContentSize().width / 2;
    float radius2 = target->getContentSize().width / 2;
    return distance < (radius1 + radius2);
}
#endif