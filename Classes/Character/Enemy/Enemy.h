/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.h
 * File Function: ����������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "cocos2d.h"
#include "Character/CharacterBase.h"
#include "../Hero/Hero.h"

// �����ս��Զ�̹���
const int Ranged_Enemy = 0;
const int Melee_Enemy = 1;
const int amount_of_healing = 10;

enum class Direction 
{
    UP, DOWN, LEFT, RIGHT
};


 //ö�ٵ��˵�״̬
enum class EnemyState
{
    PATROL,   //Ѳ��
    CHASE,    //׷��
    RETURN,   //���س�����
    STAY      //ֹͣ
};

class Enemy : public CharacterBase {
public:
    // ���캯������������
    Enemy();
    virtual ~Enemy();


    // ���˳�ʼ���������ɴ����ʼλ�õȲ���
    virtual bool init(const cocos2d::Vec2& initPosition)override;
    void setInitData(int level);

    // create �������ڴ�������ʼ������ʵ��
    static Enemy* create(const cocos2d::Vec2& initPosition);

    // ���˸��·�����������˵��߼������ƶ���������
    virtual void update(float delta)override;
    void takeDamage(float damage);
    void updateDamageLabel(int damage);
    void updateERLabel();
    void Freeze();
    void hideFreezeSprite(float dt);
    void updateHero();
    void updateHealthFill();

    // ���״̬���
    bool isAlive() const;
    void setDeath();

    //��ȡ��ɫԪ������
    CharacterElement getElement();

    // ���˹�������
    virtual void attack()override;
    void setAttackMethods(bool method);
    void attackWithPunch();
    void attackWithPistol();
    // ��ײ�����أ��ɸ��ݾ�����ײ�߼���չ��
    bool checkCollision(cocos2d::Sprite* target);

    //�������Ƿ����ж���Χ��
    bool isAttacking;
    void setPlayer(Hero* player);
    Hero* getPlayer();
    void setRadius(float radius);
    bool isHeroExist(float dt);

    //����Ѳ�߽߱�
    void setPatrolRange(float X, float Y);

    // �ص�������ָ�����ֵ 
    virtual void Recover(float delta)override;

    bool task_alive = 1;
private:
    int m_health;
    int m_full_health;
    int m_attackPower;
    bool m_isAlive;
    bool isRanged;

    // ȭͷ�Ĺ�����Χ
    float attackRange;
    // ȭͷ�Ĺ�����ȴʱ��
    float attackCooldown;
    // ʣ��Ĺ�����ȴʱ��
    float remainingCooldown;

    //��ұ���
    Hero* player;

    //����״̬
    EnemyState currentState;
    Direction currentDirection;

    //�����뾶
    float radius;

    //Ѳ�߷�Χ
    float rangedX, rangedY;
    float m_moveSpeed;

    //Ѳ�߷���
    int dirX, dirY;

    // ����Ѫ��
    cocos2d::Sprite* healthBg;
    cocos2d::Sprite* healthFill;

    //���˵�Ѳ�ߺ���
    void patrol(float delta);

    // ���Ƶ��˸���״̬���߼�
    // Ѳ��
    void patrolLogic(float& dt, float& distanceToPlayer);
    // ׷��
    void chaseLogic(float& dt, cocos2d::Vec2& playerPos, cocos2d::Vec2& enemyPos, cocos2d::Vec2& directionToPlayer, float& distanceToPlayer);
    void handleChasingMovement(float& dt, cocos2d::Vec2& directionToPlayer);
    // ԭ�ز�����������
    void stayLogic(float& distanceToPlayer);
    // ���س�����
    void returnToSpawnLogic(float& dt, cocos2d::Vec2& enemyPos, float& distanceToPlayer, cocos2d::Vec2& playerPos);
    void handleReturnToSpawnMovement(float& dt, cocos2d::Vec2& enemyPos);

    // ��ײ�߼�
    bool checkCollisionWithScene(cocos2d::Vec2& targetPosition);

    // ���˵��ƶ��߼����ɸ�����Ҫ��չ����׷����ҵ�
    void moveLogic(float dt);


    // ���˵Ĺ����߼����ɸ�����Ҫ��չ
    void checkAndHandleCollision();
    bool checkHeroCollision(cocos2d::Node* target);
    void onCollisionWithHero(cocos2d::Node* player);
    void attackLogic();


    // ���˵� AI �߼����ɸ�����Ҫ��չ
    void aiLogic();

    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;



    // ���˶�����ط���
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    cocos2d::Animation* createWalkUpAnimation();
    cocos2d::Animation* createWalkDownAnimation();
    cocos2d::Animation* createWalkLeftAnimation();
    cocos2d::Animation* createWalkRightAnimation();
    cocos2d::Animation* createDeathAnimation();
    cocos2d::Animation* createAttackDownAnimation();
    cocos2d::Animation* createAttackUpAnimation();
    cocos2d::Animation* createAttackLeftAnimation();
    cocos2d::Animation* createAttackRightAnimation();

};

#endif