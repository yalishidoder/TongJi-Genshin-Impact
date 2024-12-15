/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: 主人公类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#ifndef __HERO_H__
#define __HERO_H__

#include "cocos2d.h"
#include "Weapon/Weapon.h"
#include "Weapon/Switcher.h"
USING_NS_CC;

class Character : public cocos2d::Sprite {
public:
    // 构造函数与析构函数
    Character();
    virtual ~Character();

    // create 方法用于创建并初始化角色实例
    static Character* create(const cocos2d::Vec2& initPosition);

    // 角色初始化方法，可传入初始位置等参数
    virtual bool init(const cocos2d::Vec2& initPosition);

    // 角色移动相关方法
    void moveTo(const cocos2d::Vec2& targetPosition);
    void moveBy(const cocos2d::Vec2& offset);
    void Character::update(float dt);

    // 角色动画相关方法
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    cocos2d::Animation* Character::createWalkUpAnimation();
    cocos2d::Animation* Character::createWalkDownAnimation();
    cocos2d::Animation* Character::createWalkLeftAnimation();
    cocos2d::Animation* Character::createWalkRightAnimation();



    // 角色属性相关方法，比如设置和获取生命值、攻击力等
    void setHealth(int health);
    int getHealth();
    void setAttackPower(int attackPower);
    int getAttackPower();
    bool getGender();
    void setGender(bool is_male);

    // 角色状态相关，例如是否存活等判断
    bool isAlive();

    // 碰撞检测相关（可根据具体碰撞逻辑扩展）
    bool checkCollision(Character* otherCharacter);
        
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

    int m_sleepiness;
    int m_health;
    int m_attackPower;
    int m_heroism;
    bool m_ismale;

    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;

    WeaponSwitcher* m_weaponSwitcher;
};
#endif