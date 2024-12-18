#ifndef __PLAYER_PANEL_H__
#define __PLAYER_PANEL_H__

#include "cocos2d.h"
#include "Hero/Hero.h"
#include "ui/CocosGUI.h"

class PlayerPanel : public cocos2d::Node {
public:
    // ����PlayerPanelʵ���ľ�̬����
    static PlayerPanel* create();

    // ��ʼ������
    virtual bool init();

    // ����UI��Ϣ
    void updateInfo();

    // ���ý�ɫ
    void setHero(Hero* hero);

private:
    Hero* m_hero; // ��ɫָ��

    Inventory* m_inventory; // ����ָ��

    cocos2d::ui::ScrollView* m_scrollView; // ������ͼ
    cocos2d::ui::Layout* m_inventoryLayout; // �����Ʒ�Ĳ���

    // ������ʾ��Ʒ�Ľڵ�
    cocos2d::Node* createItemNode(Item* item);
    // ʹ����Ʒ���߼�����
    void useItem(Item* item);
};

#endif // __PLAYER_PANEL_H__