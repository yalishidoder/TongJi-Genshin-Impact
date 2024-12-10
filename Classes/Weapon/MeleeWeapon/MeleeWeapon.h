/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MeleeWeapon.h
 * File Function: ��ս����������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "Weapon/Weapon.h"


class MeleeWeapon : public Weapon {
public:
    // ���캯������������
    MeleeWeapon();
    virtual ~MeleeWeapon();


    // ��ս������ʼ���������ɴ����ʼ���ԵȲ���
    virtual bool init();


    // create �������ڴ�������ʼ����ս����ʵ��
    static MeleeWeapon* create();


    // ʵ�ֽ�ս�����Ĺ�������
    virtual void attack() override;


private:
    // ��ս�������е����Ի򷽷����ɸ�����Ҫ���
    // ���磬��ս�����Ĺ��������򹥻���Ч
    void playAttackAnimation();
};