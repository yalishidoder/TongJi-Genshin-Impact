/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MeleeWeapon.cpp
 * File Function: 近战武器类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "MeleeWeapon.h"


MeleeWeapon::MeleeWeapon() {
}


MeleeWeapon::~MeleeWeapon() {
}


bool MeleeWeapon::init() {
    if (!Weapon::init()) {
        return false;
    }


    // 可以在这里加载近战武器的初始外观纹理等
    // 例如：setTexture("melee_weapon_default.png");


    return true;
}


MeleeWeapon* MeleeWeapon::create() {
    auto weapon = new (std::nothrow) MeleeWeapon();
    if (weapon && weapon->init()) {
        weapon->autorelease();
        return weapon;
    }
    CC_SAFE_DELETE(weapon);
    return nullptr;
}


void MeleeWeapon::attack() {
    // 近战武器的攻击逻辑
    playAttackAnimation();
    // 这里可以添加近战武器的伤害计算、碰撞检测等逻辑


    // 例如，假设角色和敌人都在场景中，可以进行碰撞检测
    // auto characters = getParent()->getChildren();
    // for (auto character : characters) {
    //     // 进行碰撞检测和伤害计算
    // }
}


void MeleeWeapon::playAttackAnimation() {
    // 实现近战武器的攻击动画，例如闪烁效果或挥砍动作
    auto fadeOut = cocos2d::FadeOut::create(0.1f);
    auto fadeIn = cocos2d::FadeIn::create(0.1f);
    auto sequence = cocos2d::Sequence::create(fadeOut, fadeIn, nullptr);
    runAction(sequence);
}