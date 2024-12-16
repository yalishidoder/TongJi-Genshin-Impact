#ifndef __BAYONET_H__
#define __BAYONET_H__
#include "cocos2d.h"

class Bayonet : public cocos2d::Sprite {
public:
    Bayonet();
    ~Bayonet();

    // ��̬��������
    static Bayonet* create(const std::string& textureFileName = "bayonet.png");

    // ��ʼ������
    bool init(const std::string& textureFileName);

    // ���º���
    void update(float delta);

    // ��������
    void attack();

    // ���ʹ�������˵���ײ
    void checkAndHandleCollision();
    bool checkCollision(cocos2d::Node* target);
    void onCollisionWithEnemy(cocos2d::Node* enemy);

protected:
    // �̵��Ĺ�����Χ
    float attackRange;
    // �̵��Ĺ�����
    int attackPower;
    // �̵��Ĺ�����ȴʱ��
    float attackCooldown;
    // ʣ��Ĺ�����ȴʱ��
    float remainingCooldown;
    // �ӿ�����
    cocos2d::Animation* swingAnimation;
};

#endif