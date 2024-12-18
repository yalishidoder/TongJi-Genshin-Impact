#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "cocos2d.h"
#include "Item/item.h"

class Inventory : public cocos2d::Ref {
public:
    // 创建Inventory实例
    static Inventory* create();

    // 初始化方法
    virtual bool init();

    // 向背包中添加物品
    void addItem(Item* item);

    // 从背包中移除物品
    void removeItem(Item* item);

    // 根据名称获取物品
    Item* getItem(const std::string& name);

    // 获取所有的物品
    const std::vector<Item*>& getAllItem() const;

    // 装备物品
    void equipItem(Item* item);

private:
    std::vector<Item*> m_items; // 背包中包含的物品列表
};

#endif // __INVENTORY_H__

