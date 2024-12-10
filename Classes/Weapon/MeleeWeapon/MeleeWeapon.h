/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MeleeWeapon.h
 * File Function: 近战武器类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "Weapon/Weapon.h"


class MeleeWeapon : public Weapon {
public:
    // 构造函数与析构函数
    MeleeWeapon();
    virtual ~MeleeWeapon();


    // 近战武器初始化方法，可传入初始属性等参数
    virtual bool init();


    // create 方法用于创建并初始化近战武器实例
    static MeleeWeapon* create();


    // 实现近战武器的攻击方法
    virtual void attack() override;


private:
    // 近战武器特有的属性或方法，可根据需要添加
    // 例如，近战武器的攻击动画或攻击特效
    void playAttackAnimation();
};