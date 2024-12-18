#include "Inventory.h"

// 创建Inventory实例
Inventory* Inventory::create()
{
    auto inventory = new (std::nothrow) Inventory();
    if (inventory && inventory->init()) {
        inventory->autorelease();
        return inventory;
    }
    CC_SAFE_DELETE(inventory);
    return nullptr;
}

// 初始化方法，清空物品列表
bool Inventory::init()
{
    m_items.clear();
    return true;
}

// 向背包中添加物品
void Inventory::addItem(Item* item)
{
    m_items.push_back(item);
    // 注意：添加的物品指针需要确保其生命周期管理正确
}

// 从背包中移除物品
void Inventory::removeItem(Item* item)
{
    auto iter = std::find(m_items.begin(), m_items.end(), item);
    if (iter != m_items.end()) {
        m_items.erase(iter);
    }
}

// 根据名称获取物品，返回第一个匹配的物品
Item* Inventory::getItem(const std::string& name)
{
    for (auto item : m_items) {
        if (item->getName() == name) {
            return item;
        }
    }
    return nullptr;
}

// 获取所有的物品
const std::vector<Item*>& Inventory::getAllItem() const
{
    return m_items;
}

// 装备物品，根据物品类型进行不同处理
void Inventory::equipItem(Item* item)
{
    // 现在在考虑把装备武器单独提取出来
    // 其余物品的装备逻辑就放在背包中
}