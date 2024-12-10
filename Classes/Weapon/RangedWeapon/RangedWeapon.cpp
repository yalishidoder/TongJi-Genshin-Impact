/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     RangedWeapon.cpp
 * File Function: 远程武器类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "RangedWeapon.h"


RangedWeapon::RangedWeapon()
    : m_ammoCount(10)
    , m_fireRate(1.0f)
{
}


RangedWeapon::~RangedWeapon() {
}


bool RangedWeapon::init() {
    if (!Weapon::init()) {
        return false;
    }


    // 可以在这里加载远程武器的初始外观纹理等
    // 例如：setTexture("ranged_weapon_default.png");


    return true;
}


RangedWeapon* RangedWeapon::create() {
    auto weapon = new (std::nothrow) RangedWeapon();
    if (weapon && weapon->init()) {
        weapon->autorelease();
        return weapon;
    }
    CC_SAFE_DELETE(weapon);
    return nullptr;
}


void RangedWeapon::attack() {
    if (m_ammoCount > 0) {
        fireProjectile();
        m_ammoCount--;
    }
}


void RangedWeapon::fireProjectile() {
    // 实现远程武器发射投射物的逻辑
    // 例如，创建一个子弹并添加到场景中
    auto bullet = cocos2d::Sprite::create("bullet.png");
    bullet->setPosition(getPosition());


    // 给子弹设置一个移动动作，例如直线飞行
    auto moveAction = cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(500, 0));
    bullet->runAction(moveAction);


    // 将子弹添加到场景中
    getParent()->addChild(bullet);


    // 这里可以添加子弹的碰撞检测和伤害计算等逻辑
    // 例如，假设场景中有敌人，可以进行碰撞检测
    // auto enemies = getParent()->getChildren();
    // for (auto enemy : enemies) {
    //     // 进行碰撞检测和伤害计算
    // }
}