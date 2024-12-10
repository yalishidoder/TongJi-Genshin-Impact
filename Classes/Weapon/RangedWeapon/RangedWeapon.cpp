/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     RangedWeapon.cpp
 * File Function: Զ��������ʵ��
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


    // �������������Զ�������ĳ�ʼ��������
    // ���磺setTexture("ranged_weapon_default.png");


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
    // ʵ��Զ����������Ͷ������߼�
    // ���磬����һ���ӵ�����ӵ�������
    auto bullet = cocos2d::Sprite::create("bullet.png");
    bullet->setPosition(getPosition());


    // ���ӵ�����һ���ƶ�����������ֱ�߷���
    auto moveAction = cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(500, 0));
    bullet->runAction(moveAction);


    // ���ӵ���ӵ�������
    getParent()->addChild(bullet);


    // �����������ӵ�����ײ�����˺�������߼�
    // ���磬���賡�����е��ˣ����Խ�����ײ���
    // auto enemies = getParent()->getChildren();
    // for (auto enemy : enemies) {
    //     // ������ײ�����˺�����
    // }
}