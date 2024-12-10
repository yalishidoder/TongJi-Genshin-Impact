/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MeleeWeapon.cpp
 * File Function: ��ս������ʵ��
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


    // ������������ؽ�ս�����ĳ�ʼ��������
    // ���磺setTexture("melee_weapon_default.png");


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
    // ��ս�����Ĺ����߼�
    playAttackAnimation();
    // ���������ӽ�ս�������˺����㡢��ײ�����߼�


    // ���磬�����ɫ�͵��˶��ڳ����У����Խ�����ײ���
    // auto characters = getParent()->getChildren();
    // for (auto character : characters) {
    //     // ������ײ�����˺�����
    // }
}


void MeleeWeapon::playAttackAnimation() {
    // ʵ�ֽ�ս�����Ĺ���������������˸Ч����ӿ�����
    auto fadeOut = cocos2d::FadeOut::create(0.1f);
    auto fadeIn = cocos2d::FadeIn::create(0.1f);
    auto sequence = cocos2d::Sequence::create(fadeOut, fadeIn, nullptr);
    runAction(sequence);
}