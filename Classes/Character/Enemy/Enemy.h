/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.h
 * File Function: 敌人类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"
#include "../Hero/Hero.h"

//枚举敌人的状态
enum class EnemyState 
{
    PATROL,   //巡逻
    CHASE,    //追逐
    RETURN    //返回出生点
};

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
    virtual void update(float delta)override;


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

    //检测玩家是否在判定范围中
    bool isAttacking;
    void setPlayer(Character* player);
    void setRadius(float radius);
    bool isHeroExist(float dt);

    //设置巡逻边界
    void setPatrolRange(float X, float Y);

private:
    int m_health;
    int m_attackPower;
    bool m_isAlive;

    //玩家变量
    Character* player;  

    //敌人状态
    EnemyState currentState;

    //搜索半径
    float radius; 

    //巡逻范围
    float rangedX, rangedY;
    float m_moveSpeed;

    //巡逻方向
    int dirX, dirY;

    //敌人出生地
    cocos2d::Vec2 spawnPoint;

    //敌人的巡逻函数
    void patrol(float delta);

    // 敌人的移动逻辑，可根据需要扩展，如追逐玩家等
    void moveLogic(float dt);


    // 敌人的攻击逻辑，可根据需要扩展
    void attackLogic();


    // 敌人的 AI 逻辑，可根据需要扩展
    void aiLogic();
};