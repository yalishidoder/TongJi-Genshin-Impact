/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.h
 * File Function: 敌人类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "cocos2d.h"
#include "Character/CharacterBase.h"
#include "../Hero/Hero.h"

// 定义近战与远程攻击
const int Ranged_Enemy = 0;
const int Melee_Enemy = 1;
const int amount_of_healing = 10;

enum class Direction 
{
    UP, DOWN, LEFT, RIGHT
};


 //枚举敌人的状态
enum class EnemyState
{
    PATROL,   //巡逻
    CHASE,    //追逐
    RETURN,   //返回出生点
    STAY      //停止
};

class Enemy : public CharacterBase {
public:
    // 构造函数与析构函数
    Enemy();
    virtual ~Enemy();


    // 敌人初始化方法，可传入初始位置等参数
    virtual bool init(const cocos2d::Vec2& initPosition)override;
    void setInitData(int level);

    // create 方法用于创建并初始化敌人实例
    static Enemy* create(const cocos2d::Vec2& initPosition);

    // 敌人更新方法，处理敌人的逻辑，如移动、攻击等
    virtual void update(float delta)override;
    void takeDamage(float damage);
    void updateDamageLabel(int damage);
    void updateERLabel();
    void Freeze();
    void hideFreezeSprite(float dt);
    void updateHero();
    void updateHealthFill();

    // 存活状态相关
    bool isAlive() const;
    void setDeath();

    //获取角色元素属性
    CharacterElement getElement();

    // 敌人攻击方法
    virtual void attack()override;
    void setAttackMethods(bool method);
    void attackWithPunch();
    void attackWithPistol();
    // 碰撞检测相关（可根据具体碰撞逻辑扩展）
    bool checkCollision(cocos2d::Sprite* target);

    //检测玩家是否在判定范围中
    bool isAttacking;
    void setPlayer(Hero* player);
    Hero* getPlayer();
    void setRadius(float radius);
    bool isHeroExist(float dt);

    //设置巡逻边界
    void setPatrolRange(float X, float Y);

    // 回到出生点恢复生命值 
    virtual void Recover(float delta)override;

    bool task_alive = 1;
private:
    int m_health;
    int m_full_health;
    int m_attackPower;
    bool m_isAlive;
    bool isRanged;

    // 拳头的攻击范围
    float attackRange;
    // 拳头的攻击冷却时间
    float attackCooldown;
    // 剩余的攻击冷却时间
    float remainingCooldown;

    //玩家变量
    Hero* player;

    //敌人状态
    EnemyState currentState;
    Direction currentDirection;

    //搜索半径
    float radius;

    //巡逻范围
    float rangedX, rangedY;
    float m_moveSpeed;

    //巡逻方向
    int dirX, dirY;

    // 敌人血条
    cocos2d::Sprite* healthBg;
    cocos2d::Sprite* healthFill;

    //敌人的巡逻函数
    void patrol(float delta);

    // 控制敌人各种状态的逻辑
    // 巡逻
    void patrolLogic(float& dt, float& distanceToPlayer);
    // 追逐
    void chaseLogic(float& dt, cocos2d::Vec2& playerPos, cocos2d::Vec2& enemyPos, cocos2d::Vec2& directionToPlayer, float& distanceToPlayer);
    void handleChasingMovement(float& dt, cocos2d::Vec2& directionToPlayer);
    // 原地不动（攻击）
    void stayLogic(float& distanceToPlayer);
    // 返回出生点
    void returnToSpawnLogic(float& dt, cocos2d::Vec2& enemyPos, float& distanceToPlayer, cocos2d::Vec2& playerPos);
    void handleReturnToSpawnMovement(float& dt, cocos2d::Vec2& enemyPos);

    // 碰撞逻辑
    bool checkCollisionWithScene(cocos2d::Vec2& targetPosition);

    // 敌人的移动逻辑，可根据需要扩展，如追逐玩家等
    void moveLogic(float dt);


    // 敌人的攻击逻辑，可根据需要扩展
    void checkAndHandleCollision();
    bool checkHeroCollision(cocos2d::Node* target);
    void onCollisionWithHero(cocos2d::Node* player);
    void attackLogic();


    // 敌人的 AI 逻辑，可根据需要扩展
    void aiLogic();

    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;



    // 敌人动画相关方法
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    cocos2d::Animation* createWalkUpAnimation();
    cocos2d::Animation* createWalkDownAnimation();
    cocos2d::Animation* createWalkLeftAnimation();
    cocos2d::Animation* createWalkRightAnimation();
    cocos2d::Animation* createDeathAnimation();
    cocos2d::Animation* createAttackDownAnimation();
    cocos2d::Animation* createAttackUpAnimation();
    cocos2d::Animation* createAttackLeftAnimation();
    cocos2d::Animation* createAttackRightAnimation();

};

#endif