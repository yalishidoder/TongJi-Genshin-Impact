#include "cocos2d.h"
#include <vector>
#include "Weapon.h"


class WeaponSwitcher : public cocos2d::Node {
public:
    // 构造函数与析构函数
    WeaponSwitcher();
    virtual ~WeaponSwitcher();


    // 初始化方法
    bool init();


    // 创建方法
    static WeaponSwitcher* create();


    // 添加武器到武器库
    void addWeapon(Weapon* weapon);


    // 移除武器
    void removeWeapon(Weapon* weapon);


    // 切换到下一个武器
    void switchToNextWeapon();


    // 切换到上一个武器
    void switchToPreviousWeapon();


    // 获取当前武器
    Weapon* getCurrentWeapon();


private:
    std::vector<Weapon*> m_weapons;
    int m_currentWeaponIndex;
};