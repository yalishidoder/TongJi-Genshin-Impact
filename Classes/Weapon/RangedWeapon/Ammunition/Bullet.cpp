#include "Bullet.h"


Bullet::Bullet()
    :h_owner(nullptr)
    ,e_owner(nullptr)
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
    //默认子弹纹理
    this->setTexture("Weapon/Ammunition/bullet_default.png");

    // 设置子弹位置
    this->setPosition(position);

    // 设置子弹速度和生命周期
    this->setVelocity(velocity);
    this->setLifeTime(2.0f); // 子弹存活时间2秒
    this->_remainingLife = this->_lifeTime;

    // 设置子弹等级
    this->setLevel(level);

    // 添加更新调度
    this->scheduleUpdate();

    return true;
}

void Bullet::setOwner(Hero* player)
{
    h_owner = player;
}

void Bullet::setOwner(Enemy* enemy)
{
    e_owner = enemy;
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

// 碰撞检测与处理函数
void Bullet::checkAndHandleCollision()
{
    // 获取当前场景
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    if (currentScene)
    {
        // 由玩家射出
        if (h_owner)
        {
            // 获取场景中的所有敌人
            auto enemies = currentScene->getChildren();
            for (auto enemy : enemies)
            {
                auto temp = dynamic_cast<Enemy*>(enemy);
                if (temp && temp->isAlive())
                {
                    if (this->checkCollision(temp))
                    {
                        // 处理碰撞逻辑
                        onCollisionWithEnemy(temp);
                    }
                }
            }
        }
        // 子弹由敌人射出
        else if (e_owner) {
            if (e_owner->getPlayer()&& e_owner->getPlayer()->isAlive()) {
                // 获取场景中的玩家
                if (this->checkCollision(e_owner->getPlayer()))
                {
                    // 处理碰撞逻辑
                    onCollisionWithEnemy(e_owner->getPlayer(), e_owner);
                }

            }
        }
    }
}

// 碰撞处理逻辑
void Bullet::onCollisionWithEnemy(cocos2d::Node* targetEnemy, Enemy* attack_enemy)
{
    auto enemy = dynamic_cast<Enemy*>(targetEnemy);
    auto player = dynamic_cast<Hero*>(targetEnemy);
    if(enemy)
    {
        // 减少敌人的生命值
        enemy->takeDamage((this->getLevel() + 1.5) * 5);
    }
    else if (player) 
    {
        // 减少玩家的生命值
        player->takeDamage((this->getLevel() + 1.5) * 2, attack_enemy);
    }

    // 销毁子弹
    this->removeFromParentAndCleanup(true);
}

// 完善后的碰撞检测函数
#if 0
bool Bullet::checkCollision(cocos2d::Node* target) {
    // 获取子弹和目标的边界框
    cocos2d::Rect bulletBoundingBox = this->getBoundingBox();
    cocos2d::Rect targetBoundingBox = target->getBoundingBox();

    // 精确碰撞检测，考虑边界框是否相交
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
    return distance < (radius1 + radius2 - 5);
}
#endif