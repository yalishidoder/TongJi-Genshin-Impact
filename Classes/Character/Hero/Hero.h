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
#include "Weapon/MeleeWeapon/Bayonet.h"
#include "Inventory/Inventory.h"
#include <fstream>
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

    // 更新升级显示
    void updateLevelUpLabel();

    // 背包相关
    void setInventory(Inventory* inventory);
    Inventory* getInventory() const;

    // 武器
    void equipWeapon(Weapon* weapon);
    Weapon* getEquippedWeapon() const;

    // 攻击方法
    virtual void attack()override;
    void attackWithBullet(const Vec2& position);
    void attackWithBayonet(float angle = 0);

    // 角色动画相关方法
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    void resetAnimationCache();
    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;
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

    // 武器相关
    bool m_isBayonetGet;
    bool m_isBulletGet;
    bool m_isBayonetChosen;
    bool m_isBulletChosen;
    void ChangeToBayonet();
    void ChangeToBullet();


    // Z技能相关
    void SkillZ();
    bool m_isZSkillUnlock = 1;
    float m_skillZCoolDownTime;     //冷却时间
    bool m_isSkillZOnCoolDown;      //是否冷却完毕
    
    // X技能相关
    void SkillX();
    bool m_isXSkillUnlock = 1;
    float m_skillXCoolDownTime;     //冷却时间
    bool m_isSkillXOnCoolDown;      //是否冷却完毕

    // C技能相关
    void SkillC();
    bool m_isCSkillUnlock = 1;
    float m_skillCCoolDownTime;     //冷却时间
    bool m_isSkillCOnCoolDown;      //是否冷却完毕

    void onMouseMove(cocos2d::EventMouse* event);

    cocos2d::Label* levelupLabel;
    
    //  角色存档相关
    void saveProfile(const std::string& filename);
    void loadProfile(const std::string& filename);

private:
    bool m_isAlive;
    bool Upgrading;

    float m_speed;

    int MaxLevel;
    int m_sleepiness;
    int m_heroism;
    int m_exp;
    int m_expToLevelUp;
    bool m_ismale;

    

    Inventory* m_inventory;
    Weapon* m_equippedWeapon;

    WeaponSwitcher* m_weaponSwitcher;
    Bayonet* m_bayonet;
};
#endif