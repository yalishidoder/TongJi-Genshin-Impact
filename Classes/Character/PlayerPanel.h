#ifndef __PLAYER_PANEL_H__
#define __PLAYER_PANEL_H__

#include "cocos2d.h"
#include "Hero/Hero.h"
#include "ui/CocosGUI.h"

class PlayerPanel : public cocos2d::Node {
public:

    // 创建PlayerPanel实例的静态方法
    static PlayerPanel* create();

    // 初始化方法
    virtual bool init();

    // 初始化ui组件
    void initUi();

    // 更新UI信息
    void updateInfo();

    // 设置角色
    void setHero(Hero* hero);

    // 更改角色性别
    void changeHeroGender();
    
private:
    Hero* m_hero; // 角色指针

    Inventory* m_inventory; // 背包指针

    cocos2d::ui::ScrollView* m_scrollView; // 滚动视图
    cocos2d::ui::Layout* m_inventoryLayout; // 存放物品的布局

    // 创建表示物品的节点
    cocos2d::Node* createItemNode(Item* item);
    // 使用物品的逻辑处理
    void useItem(Item* item);
};

#endif // __PLAYER_PANEL_H__