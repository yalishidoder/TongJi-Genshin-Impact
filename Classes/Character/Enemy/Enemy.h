/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.h
 * File Function: ����������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "cocos2d.h"


class Enemy : public cocos2d::Sprite {
public:
    // ���캯������������
    Enemy();
    virtual ~Enemy();


    // ���˳�ʼ���������ɴ����ʼλ�õȲ���
    bool init(const cocos2d::Vec2& initPosition);


    // create �������ڴ�������ʼ������ʵ��
    static Enemy* create(const cocos2d::Vec2& initPosition);


    // ���˸��·�����������˵��߼������ƶ���������
    void update(float dt);


    // �����ƶ���ط���
    void moveTo(const cocos2d::Vec2& targetPosition);
    void moveBy(const cocos2d::Vec2& offset);


    // ���˹�������
    void attack();


    // ����������ط������������úͻ�ȡ����ֵ����������
    void setHealth(int health);
    int getHealth();
    void setAttackPower(int attackPower);
    int getAttackPower();


    // ����״̬��أ������Ƿ�����ж�
    bool isAlive();


    // ��ײ�����أ��ɸ��ݾ�����ײ�߼���չ��
    bool checkCollision(cocos2d::Sprite* target);


private:
    int m_health;
    int m_attackPower;
    bool m_isAlive;


    float m_moveSpeed;


    // ���˵��ƶ��߼����ɸ�����Ҫ��չ����׷����ҵ�
    void moveLogic(float dt);


    // ���˵Ĺ����߼����ɸ�����Ҫ��չ
    void attackLogic();


    // ���˵� AI �߼����ɸ�����Ҫ��չ
    void aiLogic();
};