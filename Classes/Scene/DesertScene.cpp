#include "DesertScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* DesertScene::createScene()
{
    return DesertScene::create();
}
// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainSceneScene.cpp\n");
}

bool DesertScene::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    /////////////////////////////
    // 添加一个退出按钮    this->addChild(menu, 1);
    /////////////////////////////
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(DesertScene::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
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
    /////////////////////
    //加载沙漠地图
    ////////////////////
    std::string file = "Scene/desertMap/desert.tmx";
    desertmap = TMXTiledMap::create(file);

    if (!desertmap) {
        CCLOG("Failed to load TMX map: %s", file.c_str());
        return false;
    }
    CCLOG("desertMap loaded successfully!");
    desertmap->setScale(1.0f);

    desertmap->setAnchorPoint(Vec2(0.5f, 0.5f));
    desertmap->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    this->addChild(desertmap);
    //////////////////////////
    // 地图左下角在屏幕上的实际位置
    float desertmapOriginX = desertmap->getPositionX() - (desertmap->getContentSize().width * desertmap->getScale() * desertmap->getAnchorPoint().x);
    float desertmapOriginY = desertmap->getPositionY() - (desertmap->getContentSize().height * desertmap->getScale() * desertmap->getAnchorPoint().y);
    /////////////////////////

    ///////////////////////////
    // 添加一个标签"Desert" 
    //////////////////////////
    auto label = Label::createWithTTF("Desert", "fonts/Marker Felt.ttf", 24);
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
    //////////////////
    //加载角色
    //////////////////
    auto objectGroup = desertmap->getObjectGroup("flag");
    if (objectGroup) {
        auto firstFlag = objectGroup->getObject("firstflag");
        if (!firstFlag.empty()) {
            float x = firstFlag["x"].asFloat();
            float y = firstFlag["y"].asFloat();
            CCLOG("Character spawn position: x = %.2f, y = %.2f", x, y);
            // 创建角色并放置在出生位置
            character = Sprite::create("CloseSelected.png");
            if (character) {
                character->setAnchorPoint(Vec2(0.5f, 0.5f));

                // 将角色传送到 firstflag 的位置
                // 计算出firstflag的屏幕坐标
                float adjustedX = desertmapOriginX + x * desertmap->getScale(); // 地图左下角 + 出生点的 x 偏移
                float adjustedY = desertmapOriginY + y * desertmap->getScale(); // 地图左下角 + 出生点的 y 偏移
                
                character->setPosition(Vec2(adjustedX, adjustedY));
                this->addChild(character);  // 将角色添加到场景中
            }
            else {
                CCLOG("Failed to load character");
            }
        }
        else {
            CCLOG("Failed to load object-firstflag");
        }
    }
    else {
        CCLOG("Failed to load objectGroup-flag");
    }

    // 实现按键移动
    // 注册新的键盘事件监听器
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
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
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    // 注册 update 函数，让它每一帧都被调用
    this->schedule([this](float dt) {
        this->update(dt);
        }, "update_key");
}
void DesertScene::update(float dt)
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
}
void DesertScene::returnToLastScene()
{
    // 使用 popScene 返回到之前的场景
    Director::getInstance()->popScene();
}

void DesertScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
