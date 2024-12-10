/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.h
 * File Function: 敌人类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"


class Enemy : public cocos2d::Sprite {
public:
    // 构造函数与析构函数
    Enemy();
    virtual ~Enemy();


    // 敌人初始化方法，可传入初始位置等参数
    bool init(const cocos2d::Vec2& initPosition);


    // create 方法用于创建并初始化敌人实例
    static Enemy* create(const cocos2d::Vec2& initPosition);


    // 敌人更新方法，处理敌人的逻辑，如移动、攻击等
    void update(float dt);


    // 敌人移动相关方法
    void moveTo(const cocos2d::Vec2& targetPosition);
    void moveBy(const cocos2d::Vec2& offset);


    // 敌人攻击方法
    void attack();


    // 敌人属性相关方法，比如设置和获取生命值、攻击力等
    void setHealth(int health);
    int getHealth();
    void setAttackPower(int attackPower);
    int getAttackPower();


    // 敌人状态相关，例如是否存活等判断
    bool isAlive();


    // 碰撞检测相关（可根据具体碰撞逻辑扩展）
    bool checkCollision(cocos2d::Sprite* target);


private:
    int m_health;
    int m_attackPower;
    bool m_isAlive;


    float m_moveSpeed;


    // 敌人的移动逻辑，可根据需要扩展，如追逐玩家等
    void moveLogic(float dt);


    // 敌人的攻击逻辑，可根据需要扩展
    void attackLogic();


    // 敌人的 AI 逻辑，可根据需要扩展
    void aiLogic();
};