/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MiniMap.h
 * File Function: ʵ��С��ͼ(���Ͻǵ��״�)���ܵ�������
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12.13
 ****************************************************************/

#ifndef MINIMAP_H
#define MINIMAP_H

#include "cocos2d.h"
#include "Character/Hero/Hero.h"
#include "Character/Enemy/Enemy.h"
USING_NS_CC;

class MiniMap : public cocos2d::Node {
public:
    // Factory method to create the MiniMap
    static MiniMap* create(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene);

    /*  
       ����С��ͼ
       ��������ͼ�ļ�������Ļ��С�������ڵ�
   */
    bool init(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene);

    /*  
       ����С��ͼ�ϵĽ�ɫλ�ò�����ͬ��
       ��������
       */
    void setCharacter();



    /*
        ͬ��С��ͼ�ϵĽ�ɫλ��
        ��������
    */
    void update(float dt);

private:
    // ����ͼ
    TMXTiledMap* map_;
    // ����ͼ�ļ���
    std::string file_;
    // ��Ļ��С
    Size visibleSize_;
    // ����ͼ�ĳ����ڵ�
    Scene* scene_;
    // С��ͼ�ڵ�
    Node* miniMap_;
    // С��ͼ���ű���
    float miniMapScale_;
    // С��ͼԲ�α߿�뾶
    float miniMapRadius_;
    // С��ͼ����λ��
    Vec2 miniMapCenter_;
    // С��ͼ�Ĳü��ڵ�
    ClippingNode* clipper_;
    // С��ͼ�ı߿�
    DrawNode* miniMapBorder_;
    // С��ͼ�ı���
    TMXTiledMap * miniMapBackground_;
    // С��ͼ������
    Character* hero_;
    // С��ͼ�ĵ���
    Enemy* demon_;
    // С��ͼ�����Ǳ�ǽڵ�
    DrawNode* heroMarker_;
    // С��ͼ�ĵ��˱�ǽڵ�
    DrawNode* demonMarker_;
};

#endif // MINIMAP_H