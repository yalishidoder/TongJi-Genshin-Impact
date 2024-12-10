/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Weapon.cpp
 * File Function: 武器基类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "Weapon.h"


Weapon::Weapon()
    : m_damage(0)
    , m_attackRange(0.0f)
{
}


Weapon::~Weapon() {
}


bool Weapon::init() {
    if (!cocos2d::Node::init()) {
        return false;
    }


    return true;
}



int Weapon::getDamage() {
    return m_damage;
}


void Weapon::setDamage(int damage) {
    m_damage = damage;
}


float Weapon::getAttackRange() {
    return m_attackRange;
}


void Weapon::setAttackRange(float range) {
    m_attackRange = range;
}