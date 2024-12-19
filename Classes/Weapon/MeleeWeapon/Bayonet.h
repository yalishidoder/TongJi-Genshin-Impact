#ifndef __BAYONET_H__
#define __BAYONET_H__
#include "cocos2d.h"

class Bayonet : public cocos2d::Sprite {
public:
    Bayonet();
    ~Bayonet();

    // 静态创建函数
    static Bayonet* create(const std::string& textureFileName = "bayonet.png");

    // 初始化函数
    bool init(const std::string& textureFileName);

    // 更新函数
    void update(float delta);

    // 攻击函数
    void attack(float angle = 0);

    // 检查和处理与敌人的碰撞
    void checkAndHandleCollision();
    bool checkCollision(cocos2d::Node* target);
    void onCollisionWithEnemy(cocos2d::Node* enemy);

    // 更新Bayonet的旋转角度
    void updateRotation();
    void setMousePosition(const cocos2d::Vec2& position);
#if 0
    cocos2d::Animation* createSwingAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    void playAnimation(const std::string& animationName);
#endif
    // 刺刀的攻击力
    int attackPower;
protected:
    // 刺刀的攻击范围
    float attackRange;
    
    // 刺刀的攻击冷却时间
    float attackCooldown;
    // 剩余的攻击冷却时间
    float remainingCooldown;
    // 挥砍动画
    cocos2d::AnimationCache* m_animationCache;
    float m_rotationAngle;
    cocos2d::Vec2 m_mousePosition;

    bool isIdle;
    bool nextDir;
};

#endif