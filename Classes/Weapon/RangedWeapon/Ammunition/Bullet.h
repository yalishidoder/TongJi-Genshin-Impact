#ifndef __BULLET_H__
#define __BULLET_H__

#include "cocos2d.h"
#include "Character/Enemy/Enemy.h"
class Bullet : public cocos2d::Sprite
{
public:
    Bullet();
    ~Bullet();

    // 创建子弹的工厂方法
    static Bullet* create(const cocos2d::Vec2& position = cocos2d::Vec2::ZERO, const cocos2d::Vec2& velocity = cocos2d::Vec2::ZERO, const int level = 1);

    // 初始化子弹
    bool init(const cocos2d::Vec2& position, const cocos2d::Vec2& velocity, const int level);

    // 更新子弹状态
    void update(float delta);

    // 设置子弹速度
    void setVelocity(const cocos2d::Vec2& velocity);

    // 获取子弹速度
    cocos2d::Vec2 getVelocity() const;

    // 设置子弹生命周期
    void setLifeTime(float lifeTime);

    // 获取子弹生命周期
    float getLifeTime() const;

    // 设置子弹等级
    void setLevel(int level);

    // 获取子弹等级
    int getLevel() const;

    // 子弹碰撞相关
    void checkAndHandleCollision();
    bool checkCollision(cocos2d::Node* target);
    void onCollisionWithEnemy(cocos2d::Node* enemy);
    

protected:
    // 子弹速度
    cocos2d::Vec2 _velocity;

    // 子弹生命周期
    float _lifeTime;

    // 子弹剩余寿命
    float _remainingLife;

    // 子弹等级
    int _level;

    
};

#endif // __BULLET_H__
