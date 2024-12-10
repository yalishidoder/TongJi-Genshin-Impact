/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Weapon.h
 * File Function: ������������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"

#ifndef __WEAPON_H__
#define __WEAPON_H__

class Weapon : public cocos2d::Node {
public:
    // ���캯������������
    Weapon();
    virtual ~Weapon();


    // ������ʼ���������ɴ����ʼ���ԵȲ���
    virtual bool init();


    // create �������ڴ�������ʼ������ʵ��
    static Weapon* create();


    // ����������������ͬ�����в�ͬʵ��
    virtual void attack()=0;


    // ��ȡ�����˺�ֵ
    int getDamage();


    // ���������˺�ֵ
    void setDamage(int damage);


    // ��ȡ����������Χ
    float getAttackRange();


    // ��������������Χ
    void setAttackRange(float range);


protected:
    int m_damage;
    float m_attackRange;
};

#endif