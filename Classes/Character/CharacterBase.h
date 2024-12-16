#ifndef __CHARACTER_BASE_H__
#define __CHARACTER_BASE_H__

#include "cocos2d.h"

// 角色的属性
enum class CharacterElement
{
    FIRE,   //火
    WATER,  //水
    ICE,    //冰
    ROCK    //岩
};

class CharacterBase : public cocos2d::Sprite 
{
public:
    // 构造函数与析构函数
    CharacterBase();
    virtual ~CharacterBase();

    // 初始化方法
    virtual bool init(const cocos2d::Vec2& initPosition) = 0;

    // 角色更新方法
    virtual void update(float delta) = 0;

    // 移动方法
    virtual void moveTo(const cocos2d::Vec2& targetPosition);
    virtual void moveBy(const cocos2d::Vec2& offset);

    // 攻击方法
    virtual void attack() = 0;

    // 获取和设置生命值
    virtual void setHealth(int health);
    virtual void setMaxHealth(int health);
    virtual int getHealth() const;
    virtual int getMaxHealth()const;

    // 获取和设置攻击力
    virtual void setAttackPower(int attackPower);
    virtual int getAttackPower() const;

    // 获取和设置等级
    virtual int getLevel()const;
    virtual void setLevel(int level);

    // 回到出生点恢复生命值 
    virtual void Recover();
protected:
    // 生命值
    int m_health;
    int m_full_health;

    // 攻击力
    int m_attackPower;

    // 等级
    int m_level;
    int MaxLevel;

    CharacterElement element;

    // 出生点
    // 所有角色返回出生点恢复所有生命值
    cocos2d::Vec2 spawnPoint;
};

#endif // __CHARACTER_BASE_H__
