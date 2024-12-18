/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.h
 * File Function: 敌人类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"
#include "Character/CharacterBase.h"
#include "../Hero/Hero.h"

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

    // 存活状态相关
    bool isAlive() const;
    void setDeath();

    //获取角色元素属性
    CharacterElement getElement();

    // 敌人攻击方法
    virtual void attack()override;

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

    // 伤害标签
    cocos2d::Label* damageLabel;
private:
    int m_health;
    int m_full_health;
    int m_attackPower;
    bool m_isAlive;

    //玩家变量
    Hero* player;

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

    // 控制敌人各种状态的逻辑
    // 巡逻
    void patrolLogic(float &dt, float &distanceToPlayer);     
    // 追逐
    void chaseLogic(float &dt, cocos2d::Vec2 &playerPos, cocos2d::Vec2 &enemyPos, cocos2d::Vec2 &directionToPlayer, float& distanceToPlayer);
    void handleChasingMovement(float& dt, cocos2d::Vec2& directionToPlayer);
    // 原地不动（攻击）
    void stayLogic(float &distanceToPlayer);
    // 返回出生点
    void returnToSpawnLogic(float &dt, cocos2d::Vec2 &enemyPos, float &distanceToPlayer, cocos2d::Vec2 &playerPos);
    void handleReturnToSpawnMovement(float& dt, cocos2d::Vec2& enemyPos);

    // 碰撞逻辑
    bool checkCollisionWithScene(cocos2d::Vec2& targetPosition);

    // 敌人的移动逻辑，可根据需要扩展，如追逐玩家等
    void moveLogic(float dt);


    // 敌人的攻击逻辑，可根据需要扩展
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
};