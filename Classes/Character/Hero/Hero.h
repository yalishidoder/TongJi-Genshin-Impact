/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: ���˹�������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#ifndef __HERO_H__
#define __HERO_H__

#include <vector>
#include "cocos2d.h"
#include "Character/CharacterBase.h"
#include "Weapon/Weapon.h"
#include "Weapon/Switcher.h"
#include "Weapon/MeleeWeapon/Bayonet.h"
#include "Inventory/Inventory.h"
#include <fstream>
USING_NS_CC;

class Enemy;
class Hero : public CharacterBase {
public:
    // ���캯������������
    Hero();
    virtual ~Hero();

    // create �������ڴ�������ʼ����ɫʵ��
    static Hero* create(const cocos2d::Vec2& initPosition);

    // ��ɫ��ʼ���������ɴ����ʼλ�õȲ���
    virtual bool init(const cocos2d::Vec2& initPosition)override;

    // ��ɫ�ƶ���ط���
    void moveTo(const cocos2d::Vec2& targetPosition);
    void moveBy(const cocos2d::Vec2& offset);
    virtual void Hero::update(float dt)override;

    // ���¸�����ʾ
    void updateLevelUpLabel();
    void takeDamage(float damage, Enemy* enemy);
    void updateDamageLabel(int damage);
    void updateERLabel();
    void Freeze();
    void hideFreezeSprite(float dt);
    void updateRecoverLabel();

    // �ص�������ָ�����ֵ 
    virtual void Recover(float delta)override;

    // ��ɫ����
    void respawnHero(int audioId);

    // ������Ϸ
    void endGame();
    
    // �������
    void setInventory(Inventory* inventory);
    Inventory* getInventory() const;

    // ����
    void equipWeapon(Weapon* weapon);
    Weapon* getEquippedWeapon() const;

    // ��������
    virtual void attack()override;
    void attackWithBullet(const Vec2& position);
    void attackWithBayonet(float angle = 0);

    // ��ɫ������ط���
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    void resetAnimationCache();
    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;
    cocos2d::Animation* Hero::createWalkUpAnimation();
    cocos2d::Animation* Hero::createWalkDownAnimation();
    cocos2d::Animation* Hero::createWalkLeftAnimation();
    cocos2d::Animation* Hero::createWalkRightAnimation();
    cocos2d::Animation* Hero::createDeathAnimation();

    // ��ɫ������ط������������úͻ�ȡ����ֵ����������
    bool getGender();
    void setGender(bool is_male);
    CharacterElement getElement();

    // ��ɫ�ȼ��뾭�����
    void addExp(int exp);
    void LevelUp();
    int calculateExpToLevelUp()const;

    // ���úͻ�ȡ�������
    void setGoldCoin(int value);
    int getGoldCoin()const;

    // ��ɫ״̬��أ������Ƿ�����ж�
    bool isAlive() const;
    void setDeath();

    // ��ײ�����أ��ɸ��ݾ�����ײ�߼���չ��
    bool checkCollision(Hero* otherHero);

    cocos2d::Vec2 m_moveDirection;
    bool m_moveUp;
    bool m_moveDown;
    bool m_moveLeft;
    bool m_moveRight;

    // �������
    bool m_isBayonetGet;
    bool m_isBulletGet;
    bool m_isBayonetChosen;
    bool m_isBulletChosen;
    void ChangeToBayonet();
    void ChangeToBullet();


    // Z�������
    void SkillZ();
    bool m_isZSkillUnlock = 1;
    float m_skillZCoolDownTime;     //��ȴʱ��
    bool m_isSkillZOnCoolDown;      //�Ƿ���ȴ���
    
    // X�������
    void SkillX();
    bool m_isXSkillUnlock = 0;
    float m_skillXCoolDownTime;     //��ȴʱ��
    bool m_isSkillXOnCoolDown;      //�Ƿ���ȴ���

    // C�������
    void SkillC();
    bool m_isCSkillUnlock = 1;
    float m_skillCCoolDownTime;     //��ȴʱ��
    bool m_isSkillCOnCoolDown;      //�Ƿ���ȴ���

    void onMouseMove(cocos2d::EventMouse* event);

    cocos2d::Label* levelupLabel;
    cocos2d::Label* weaponLabel;
    //  ��ɫ�浵���
    void saveProfile(const std::string& filename);
    void loadProfile(const std::string& filename);

private:
    bool m_isAlive;
    bool Upgrading;
    float m_speed;

    int MaxLevel;
    int m_sleepiness;
    int m_heroism;
    int m_exp;
    int m_expToLevelUp;
    int m_gold_coin;
    bool m_ismale;

    

    Inventory* m_inventory;
    Weapon* m_equippedWeapon;

    WeaponSwitcher* m_weaponSwitcher;
    Bayonet* m_bayonet;
};
#endif