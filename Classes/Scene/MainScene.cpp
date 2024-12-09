#include "MainScene.h"
#include "SimpleAudioEngine.h"

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


    return true;
}


void MainScene::menuCloseCallback(Ref* pSender)
{
    //关闭cocos2d-x游戏场景并退出应用程序
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}