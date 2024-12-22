#ifndef __CHARACTER_BASE_H__
#define __CHARACTER_BASE_H__

#include "cocos2d.h"
#include <string>

// 角色的属性
enum class CharacterElement
{
    FIRE,   //火
    WATER,  //水
    ICE,    //冰
    ROCK    //岩
};

const float FreezeTime = 1.0f;

class CharacterBase : public cocos2d::Sprite
{
public:
    // 构造函数与析构函数
    CharacterBase();
    virtual ~CharacterBase();

    // 初始化方法
    virtual bool init(const cocos2d::Vec2& initPosition) = 0;

    // 获取和设置名字
    virtual void setCharacterName(const std::string& Name);
    virtual std::string getCharacterName();

    void setSpawnPoint(const cocos2d::Vec2& position);
    cocos2d::Vec2 getSpawnPoint()const;

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

    // 设置角色元素
    virtual void setElement(CharacterElement Elementpower);

    // 回到出生点恢复生命值 
    virtual void Recover(float delta);

    // 设置控制时间与控制状态
    virtual void applyControl(float duration);
    virtual void setControlled(bool controlled);
    virtual void onControlEnd(float dt);
protected:
    // 角色特定名字
    std::string m_name;

    // 元素反应类型
    std::string ERType;

    // 生命值
    int m_health;
    int m_full_health;

    // 攻击力
    int m_attackPower;

    // 等级
    int m_level;
    int MaxLevel;

    bool isControlled;
    // 元素
    CharacterElement element;

    // 出生点
    // 所有角色返回出生点恢复所有生命值
    cocos2d::Vec2 spawnPoint;

    // 伤害标签
    cocos2d::Label* damageLabel;

    // 元素反应标签
    cocos2d::Label* ERLabel;

    // 元素反应标签
    cocos2d::Label* recoverLabel;

    cocos2d::Sprite* freezeSprite;
};

#endif // __CHARACTER_BASE_H__
