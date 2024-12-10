/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Weapon.h
 * File Function: 武器基类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"

#ifndef __WEAPON_H__
#define __WEAPON_H__

class Weapon : public cocos2d::Node {
public:
    // 构造函数与析构函数
    Weapon();
    virtual ~Weapon();


    // 武器初始化方法，可传入初始属性等参数
    virtual bool init();


    // create 方法用于创建并初始化武器实例
    static Weapon* create();


    // 武器攻击方法，不同武器有不同实现
    virtual void attack()=0;


    // 获取武器伤害值
    int getDamage();


    // 设置武器伤害值
    void setDamage(int damage);


    // 获取武器攻击范围
    float getAttackRange();


    // 设置武器攻击范围
    void setAttackRange(float range);


protected:
    int m_damage;
    float m_attackRange;
};

#endif