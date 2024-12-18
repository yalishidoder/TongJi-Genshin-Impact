#include "Item.h"

// 创建Item实例
Item* Item::create(const std::string& name, const std::string& description, ItemType type)
{
    auto item = new (std::nothrow) Item();
    if (item && item->init(name, description, type))
    {
        item->autorelease();
        return item;
    }
    CC_SAFE_DELETE(item);
    return nullptr;
}

// 初始化方法，设置物品的名称、描述和类型
bool Item::init(const std::string& name, const std::string& description, ItemType type)
{
    m_name = name;
    m_description = description;
    m_type = type;
    return true;
}

// 获取物品名称
std::string Item::getName() const
{
    return m_name;
}

// 获取物品描述
std::string Item::getDescription() const
{
    return m_description;
}

// 获取物品类型
ItemType Item::getType() const
{
    return m_type;
}

// 更新方法，用于每帧更新物品状态（当前未实现具体功能）
void Item::update(float dt)
{
    // 实现物品的更新逻辑
}