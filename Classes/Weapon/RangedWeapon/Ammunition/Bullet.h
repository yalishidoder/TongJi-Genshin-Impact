#ifndef __BULLET_H__
#define __BULLET_H__

#include "cocos2d.h"

class Bullet : public cocos2d::Sprite
{
public:
    Bullet();
    ~Bullet();

    // 创建子弹的工厂方法
    static Bullet* create(const cocos2d::Vec2& position = cocos2d::Vec2::ZERO, const cocos2d::Vec2& velocity = cocos2d::Vec2::ZERO);

    // 初始化子弹
    bool init(const cocos2d::Vec2& position, const cocos2d::Vec2& velocity);

    // 更新子弹位置
    void update(float delta);

    // 设置子弹速度
    void setVelocity(const cocos2d::Vec2& velocity);

    // 获取子弹速度
    cocos2d::Vec2 getVelocity() const;

    // 设置子弹生命周期
    void setLifeTime(float lifeTime);

    // 获取子弹生命周期
    float getLifeTime() const;

    // 子弹碰撞检测
    void onCollisionWithEnemy(cocos2d::Node* enemy);

protected:
    // 子弹速度
    cocos2d::Vec2 _velocity;

    // 子弹生命周期
    float _lifeTime;

    // 子弹剩余寿命
    float _remainingLife;
};

#endif // __BULLET_H__
