#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "cocos2d.h"
#include "Item/item.h"

class Inventory : public cocos2d::Ref {
public:
    // ����Inventoryʵ��
    static Inventory* create();

    // ��ʼ������
    virtual bool init();

    // �򱳰��������Ʒ
    void addItem(Item* item);

    // �ӱ������Ƴ���Ʒ
    void removeItem(Item* item);

    // �������ƻ�ȡ��Ʒ
    Item* getItem(const std::string& name);

    // ��ȡ���е���Ʒ
    const std::vector<Item*>& getAllItem() const;

    // װ����Ʒ
    void equipItem(Item* item);

private:
    std::vector<Item*> m_items; // �����а�������Ʒ�б�
};

#endif // __INVENTORY_H__

