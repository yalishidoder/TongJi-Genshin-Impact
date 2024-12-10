#include "MainScene.h"
#include "DesertScene.h"
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
#if 1
    /////////////////////////////
    // 添加一个退出按钮    this->addChild(menu, 1);
    /////////////////////////////
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
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

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
#endif
    ////////////////////////////
    // 加载主地图    this->addChild(map,0);
    ////////////////////////////
    std::string file = "Scene/MainScene/mainmap.tmx";
    map = TMXTiledMap::create(file);

    if (!map) {
        CCLOG("Failed to load TMX map: %s", file.c_str());
        return false;
    }
    CCLOG("Map loaded successfully!");   
    map->setScale(1.0f);
   
    map->setAnchorPoint(Vec2(0.5f, 0.5f));
    map->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    this->addChild(map);
    //////////////////////////
    // 地图左下角在屏幕上的实际位置
    float mapOriginX = map->getPositionX() - (map->getContentSize().width * map->getScale() * map->getAnchorPoint().x);
    float mapOriginY = map->getPositionY() - (map->getContentSize().height * map->getScale() * map->getAnchorPoint().y);
    /////////////////////////
    
    ///////////////////////////
    // 添加一个标签"MainScene" map->addChild(label, 1);
    //////////////////////////
    auto label = Label::createWithTTF("MainScene", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }
    //////////////////////////
    //加载角色 add 0
    //////////////////////////
    auto objectGroup_ch = map->getObjectGroup("chusheng");
    if (objectGroup_ch) {
        auto chushengObject = objectGroup_ch->getObject("initspot");
        if (!chushengObject.empty()) {
            float x = chushengObject["x"].asFloat();
            float y = chushengObject["y"].asFloat();
            CCLOG("Character spawn position: x = %.2f, y = %.2f", x, y);
            // 创建角色并放置在出生位置
            character = Sprite::create("CloseSelected.png");  
            if (character) {
                character->setAnchorPoint(Vec2(0.5f, 0.5f));

                // 计算出生点的屏幕坐标
                float adjustedX = mapOriginX + x * map->getScale(); // 地图左下角 + 出生点的 x 偏移
                float adjustedY = mapOriginY + y * map->getScale(); // 地图左下角 + 出生点的 y 偏移

                // 设置人物位置
                character->setPosition(Vec2(adjustedX, adjustedY));
                this->addChild(character);  // 将角色添加到场景中
            }
        }
    }
    else {
        CCLOG("Failed to load chchchch");
    }
    // 读取主场景中的地图传送点位置
    auto objectGroup = map->getObjectGroup("SceneSwitchPoints");
    if (objectGroup) {
        for (auto& obj : objectGroup->getObjects()) {

            Vec2 switchPos(obj.asValueMap()["x"].asFloat(), obj.asValueMap()["y"].asFloat());
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX = mapOriginX + switchPos.x * map->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY = mapOriginY + switchPos.y * map->getScale(); // 地图左下角 + 出生点的 y 偏移
            switchPos.x = adjustedX;
            switchPos.y = adjustedY;
            //输出传送点位置
            CCLOG("Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);

            std::string targetMap = obj.asValueMap()["targetMap"].asString();
            // 输出传送点目的地调试信息
            CCLOG("targetMap: %s", targetMap.c_str());   
            sceneSwitchPoints.push_back({ switchPos, targetMap });
        }
    }

    moveUp = moveDown = moveLeft = moveRight = false;
    // 注册键盘事件监听器
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        // 按键按下时设置移动状态
        if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            moveUp = true;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_S) {
            moveDown = true;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            moveLeft = true;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            moveRight = true;
        }
        };

    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        // 按键松开时停止相应的移动
        if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            moveUp = false;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_S) {
            moveDown = false;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            moveLeft = false;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            moveRight = false;
        }
        };

    // 将监听器添加到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    // 注册 update 函数，让它每一帧都被调用
    this->schedule([this](float dt) {
        this->update(dt);
        }, "update_key");
    return true;
}
void MainScene::update(float dt)
{
    // 根据按键状态来更新角色的位置
    if (moveUp) {
        character->setPosition(character->getPosition() + Vec2(0, 2));
    }
    if (moveDown) {
        character->setPosition(character->getPosition() + Vec2(0, -2));
    }
    if (moveLeft) {
        character->setPosition(character->getPosition() + Vec2(-2, 0));
    }
    if (moveRight) {
        character->setPosition(character->getPosition() + Vec2(2, 0));
    }

    // 假设 character 是角色对象
    //遍历每一个传送点信息
    for (const auto& switchPoint : sceneSwitchPoints) {
        //判断地图传送点和人物是否碰撞
        if (character->getBoundingBox().intersectsRect(Rect(switchPoint.position.x - 10, switchPoint.position.y - 10, 20, 20))) {
            // 触发切换地图
            if (switchPoint.targetMap == "desert.tmx") {
                Director::getInstance()->pushScene(DesertScene::createScene());
            }
        }
    }
}
void MainScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
