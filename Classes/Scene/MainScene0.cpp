#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "Character/Hero/Hero.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainSceneScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 添加一个“关闭”图标以退出进度。它是一个自动释放对象
    auto closeItem = MenuItemImage::create(
        "Scene/MainScene/CloseNormal.png",
        "Scene/MainScene/CloseSelected.png",
        CC_CALLBACK_1(MainScene::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'Scene/MainScene/CloseNormal.png' and 'Scene/MainScene/CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    //创建菜单，它是一个自动释放对象
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    /////////////////////////////
    // 加载主地图

    std::string file = "Scene/MainScene/mainmap.tmx";
    map = TMXTiledMap::create(file);

    if (!map) {
        CCLOG("Failed to load TMX map: %s", file.c_str());
        return false;
    }
    CCLOG("Map loaded successfully!");

    // 获取地图尺寸
    float mapWidth = map->getMapSize().width * map->getTileSize().width;
    float mapHeight = map->getMapSize().height * map->getTileSize().height;

    // 计算缩放比例
    float scaleX = visibleSize.width / mapWidth;
    float scaleY = visibleSize.height / mapHeight;
    float scale = std::min(scaleX, scaleY);
    map->setScale(scale);

    map->setAnchorPoint(Vec2(0.5f, 0.5f));
    map->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(map);


    // 添加一个标签显示“MainScene”

    auto label = Label::createWithTTF("MainScene", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        //将标签放置在屏幕中央
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height));

        //添加标签作为子图层
        this->addChild(label, 1);
    }

    // 创建角色实例并初始化位置
    auto hero = Character::create(Vec2(471,435));
    if (1) {  // 初始位置可调整
        hero->setName("hero"); // 设置角色名称
        this->addChild(hero);
        // 移动角色示例
        //hero->moveTo(Vec2(300, 300));


        // 设置属性示例
        //hero->setHealth(80);
        //hero->setAttackPower(15);
        
        // 启用更新函数
        this->scheduleUpdate();


        // 添加键盘事件监听器
        auto listener = cocos2d::EventListenerKeyboard::create();
        listener->onKeyPressed = CC_CALLBACK_2(MainScene::onKeyPressed, this);
        listener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
    return true;
}

void MainScene::update(float dt) {
    Node::update(dt);


    auto children = getChildren();
    for (auto child : children) {
        auto character = dynamic_cast<Character*>(child);
        if (character) {
            character->update(dt);
        }
    }
}

void MainScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto hero = dynamic_cast<Character*>(this->getChildByName("hero"));
    if (hero) {
        switch (keyCode) {
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_W:
        case cocos2d::EventKeyboard::KeyCode::KEY_W:
            hero->m_moveUp = true;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_S:
        case cocos2d::EventKeyboard::KeyCode::KEY_S:
            hero->m_moveDown = true;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_A:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            hero->m_moveLeft = true;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_D:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            hero->m_moveRight = true;
            break;
        default:
            break;
        }
    }
}

void MainScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto hero = dynamic_cast<Character*>(this->getChildByName("hero"));
    if (hero) {
        switch (keyCode) {
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_W:
        case cocos2d::EventKeyboard::KeyCode::KEY_W:
            hero->m_moveUp = false;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_S:
        case cocos2d::EventKeyboard::KeyCode::KEY_S:
            hero->m_moveDown = false;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_A:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            hero->m_moveLeft = false;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_D:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            hero->m_moveRight = false;
            break;
        default:
            break;
        }
    }
}

void MainScene::menuCloseCallback(Ref* pSender)
{
    //关闭cocos2d-x游戏场景并退出应用程序
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}