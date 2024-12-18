#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"

// 定义物品类型枚举
enum class ItemType
{
    MATERIAL,   // 材料
    ARMOR,      // 护甲
    CONSUMABLE, // 消耗品
    EMPTY       // 空
    // 按需添加
};

class Item : public cocos2d::Ref
{
public:
    // 创建Item实例
    static Item* create(const std::string& name = "empty", const std::string& description = "empty", ItemType type = ItemType::EMPTY);

    // 初始化方法，设置物品的名称、描述和类型
    virtual bool init(const std::string& name, const std::string& description, ItemType type);

    // 更新方法，用于每帧更新物品状态（当前未实现具体功能）
    virtual void update(float dt);

    // 获取物品名称
    std::string getName() const;

    // 获取物品描述
    std::string getDescription() const;

    // 获取物品类型
    ItemType getType() const;

protected:
    std::string m_name;        // 物品名称
    std::string m_description; // 物品描述
    ItemType m_type;           // 物品类型
};

#endif // __ITEM_H__