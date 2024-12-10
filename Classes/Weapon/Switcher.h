#include "cocos2d.h"
#include <vector>
#include "Weapon.h"


class WeaponSwitcher : public cocos2d::Node {
public:
    // ���캯������������
    WeaponSwitcher();
    virtual ~WeaponSwitcher();


    // ��ʼ������
    bool init();


    // ��������
    static WeaponSwitcher* create();


    // ���������������
    void addWeapon(Weapon* weapon);


    // �Ƴ�����
    void removeWeapon(Weapon* weapon);


    // �л�����һ������
    void switchToNextWeapon();


    // �л�����һ������
    void switchToPreviousWeapon();


    // ��ȡ��ǰ����
    Weapon* getCurrentWeapon();


private:
    std::vector<Weapon*> m_weapons;
    int m_currentWeaponIndex;
};