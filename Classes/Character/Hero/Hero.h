/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: 主人公类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#ifndef __HERO_H__
#define __HERO_H__

#include <vector>
#include "cocos2d.h"
#include "Character/CharacterBase.h"
#include "Weapon/Weapon.h"
#include "Weapon/Switcher.h"
USING_NS_CC;

class Hero : public CharacterBase {
public:
    // 构造函数与析构函数
    Hero();
    virtual ~Hero();

    // create 方法用于创建并初始化角色实例
    static Hero* create(const cocos2d::Vec2& initPosition);

    // 角色初始化方法，可传入初始位置等参数
    virtual bool init(const cocos2d::Vec2& initPosition)override;

    // 角色移动相关方法
    void moveTo(const cocos2d::Vec2& targetPosition);
    void moveBy(const cocos2d::Vec2& offset);
    virtual void Hero::update(float dt)override;

    // 攻击方法
    virtual void attack()override;
    void attackWithBullet(const Vec2& position);
    void attackWithBayonet();

    // 角色动画相关方法
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    cocos2d::Animation* Hero::createWalkUpAnimation();
    cocos2d::Animation* Hero::createWalkDownAnimation();
    cocos2d::Animation* Hero::createWalkLeftAnimation();
    cocos2d::Animation* Hero::createWalkRightAnimation();

    // 角色属性相关方法，比如设置和获取生命值、攻击力等
    bool getGender();
    void setGender(bool is_male);
    CharacterElement getElement();

    // 角色等级与经验相关
    void addExp(int exp);
    void LevelUp();
    int calculateExpToLevelUp()const;

    // 角色状态相关，例如是否存活等判断
    bool isAlive()const;

    // 碰撞检测相关（可根据具体碰撞逻辑扩展）
    bool checkCollision(Hero* otherHero);
        
    cocos2d::Vec2 m_moveDirection;
    bool m_moveUp;
    bool m_moveDown;
    bool m_moveLeft;
    bool m_moveRight;

    // 武器切换相关
    void addWeapon(Weapon* weapon);
    void removeWeapon(Weapon* weapon);
    void switchToNextWeapon();
    void switchToPreviousWeapon();
    Weapon* getCurrentWeapon();

private:
    bool m_isAlive;

    float m_speed;

    int MaxLevel;
    int m_sleepiness;
    int m_heroism;
    int m_exp;
    int m_expToLevelUp;
    bool m_ismale;

    //技能相关
    float m_skillCoolDownTime;     //冷却时间
    bool m_isSkillOnCoolDown;      //是否冷却完毕
    float m_skillDuration;         //持续时间

    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;

    WeaponSwitcher* m_weaponSwitcher;
};
#endif