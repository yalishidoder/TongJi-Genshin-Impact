/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     RangedWeapon.h
 * File Function: 远程武器类声明
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "Weapon\Weapon.h"


class RangedWeapon : public Weapon {
public:
    // 构造函数与析构函数
    RangedWeapon();
    virtual ~RangedWeapon();


    // 远程武器初始化方法，可传入初始属性等参数
    virtual bool init();


    // create 方法用于创建并初始化远程武器实例
    static RangedWeapon* create();


    // 实现远程武器的攻击方法
    virtual void attack() override;


private:
    // 远程武器特有的属性或方法，可根据需要添加
    // 例如，远程武器的弹药数量、射速等
    int m_ammoCount;
    float m_fireRate;


    void fireProjectile();
};