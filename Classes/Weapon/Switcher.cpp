#include "Switcher.h"


WeaponSwitcher::WeaponSwitcher()
    : m_currentWeaponIndex(0)
{
}


WeaponSwitcher::~WeaponSwitcher() {
    for (auto weapon : m_weapons) {
        CC_SAFE_RELEASE_NULL(weapon);
    }
    m_weapons.clear();
}


bool WeaponSwitcher::init() {
    if (!cocos2d::Node::init()) {
        return false;
    }


    return true;
}


WeaponSwitcher* WeaponSwitcher::create() {
    auto switcher = new (std::nothrow) WeaponSwitcher();
    if (switcher && switcher->init()) {
        switcher->autorelease();
        return switcher;
    }
    CC_SAFE_DELETE(switcher);
    return nullptr;
}


void WeaponSwitcher::addWeapon(Weapon* weapon) {
    m_weapons.push_back(weapon);
    weapon->retain();
}


void WeaponSwitcher::removeWeapon(Weapon* weapon) {
    auto it = std::find(m_weapons.begin(), m_weapons.end(), weapon);
    if (it != m_weapons.end()) {
        m_weapons.erase(it);
        CC_SAFE_RELEASE_NULL(weapon);


        // 如果移除的是当前武器，切换到其他武器
        if (m_currentWeaponIndex >= (int)m_weapons.size()) {
            m_currentWeaponIndex = m_weapons.size() - 1;
        }
    }
}


void WeaponSwitcher::switchToNextWeapon() {
    if (m_weapons.empty()) return;


    m_currentWeaponIndex = (m_currentWeaponIndex + 1) % m_weapons.size();


    // 可以在这里添加切换武器的动画或特效
    // 例如：playSwitchAnimation();
}


void WeaponSwitcher::switchToPreviousWeapon() {
    if (m_weapons.empty()) return;


    m_currentWeaponIndex = (m_currentWeaponIndex - 1 + m_weapons.size()) % m_weapons.size();


    // 可以在这里添加切换武器的动画或特效
    // 例如：playSwitchAnimation();
}


Weapon* WeaponSwitcher::getCurrentWeapon() {
    if (m_weapons.empty()) return nullptr;


    return m_weapons[m_currentWeaponIndex];
}