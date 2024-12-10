/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     RangedWeapon.h
 * File Function: Զ������������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "Weapon\Weapon.h"


class RangedWeapon : public Weapon {
public:
    // ���캯������������
    RangedWeapon();
    virtual ~RangedWeapon();


    // Զ��������ʼ���������ɴ����ʼ���ԵȲ���
    virtual bool init();


    // create �������ڴ�������ʼ��Զ������ʵ��
    static RangedWeapon* create();


    // ʵ��Զ�������Ĺ�������
    virtual void attack() override;


private:
    // Զ���������е����Ի򷽷����ɸ�����Ҫ���
    // ���磬Զ�������ĵ�ҩ���������ٵ�
    int m_ammoCount;
    float m_fireRate;


    void fireProjectile();
};