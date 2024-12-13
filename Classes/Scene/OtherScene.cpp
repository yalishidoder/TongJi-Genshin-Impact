#include "OtherScene.h"
#include "Scene/MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* OtherScene::createScene(const std::string& mapFile)
{
    OtherScene* ret = new OtherScene();
    if (ret && ret->init(mapFile)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainSceneScene.cpp\n");
}

bool OtherScene::init(const std::string& mapFile)
{
    if (!Scene::init())
    {
        return false;
    }
    mapname = mapFile;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    /////////////////////////////
    // 添加一个退出按钮    this->addChild(menu, 1);
    /////////////////////////////
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(OtherScene::menuCloseCallback, this));

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
    //加载小地图
    ////////////////////
    std::string file = "Scene/othermap/" + mapFile;
    othermap = TMXTiledMap::create(file);

    if (!othermap) {
        CCLOG("Failed to load TMX map: %s", file.c_str());
        return false;
    }
    CCLOG("otherMap loaded successfully!");
    othermap->setScale(1.0f);
    othermap->setAnchorPoint(Vec2(0.5f, 0.5f));
    othermap->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    this->addChild(othermap);
    //////////////////////////
    // 地图左下角在屏幕上的实际位置
    float othermapOriginX = othermap->getPositionX() - (othermap->getContentSize().width * othermap->getScale() * othermap->getAnchorPoint().x);
    float othermapOriginY = othermap->getPositionY() - (othermap->getContentSize().height * othermap->getScale() * othermap->getAnchorPoint().y);
    /////////////////////////

    ///////////////////////////
    // 添加一个标签 
    //////////////////////////
    auto label = Label::createWithTTF(mapFile, "fonts/Marker Felt.ttf", 24);
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
    auto objectGroup_flag = othermap->getObjectGroup("flag");
    if (objectGroup_flag) {
        auto firstFlag = objectGroup_flag->getObject("firstflag");
        if (!firstFlag.empty()) {
            float x = firstFlag["x"].asFloat();
            float y = firstFlag["y"].asFloat();
            CCLOG("Character spawn position: x = %.2f, y = %.2f", x, y);
            // 创建角色并放置在出生位置
            auto hero = Character::create(Vec2(500, 500));
            if (hero) {
                hero->setName("hero"); // 设置角色名称
                hero->setAnchorPoint(Vec2(0.5f, 0.5f));

                // 将角色传送到 firstflag 的位置
                // 计算出firstflag的屏幕坐标
                float adjustedX = othermapOriginX + x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
                float adjustedY = othermapOriginY + y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移

                hero->setPosition(Vec2(adjustedX, adjustedY));
                this->addChild(hero);  // 将角色添加到场景中
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
    // 读取场景中的地图传送点位置
    auto objectGroup_SceneSwitchPoints = othermap->getObjectGroup("SceneSwitchPoints");
    if (objectGroup_SceneSwitchPoints) {
        for (auto& obj : objectGroup_SceneSwitchPoints->getObjects()) {
            
            Vec2 switchPos(obj.asValueMap()["x"].asFloat(), obj.asValueMap()["y"].asFloat());
            CCLOG("before Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX_sw = othermapOriginX + switchPos.x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY_sw = othermapOriginY + switchPos.y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移
            switchPos.x = adjustedX_sw;
            switchPos.y = adjustedY_sw;
            //输出传送点位置
            CCLOG("actual Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            
            // 读取目标地图名称
            std::string targetMap = obj.asValueMap()["targetMap"].asString();
            // 输出传送点目的地调试信息
            CCLOG("targetMap: %s", targetMap.c_str());

            // 读取 no_position 属性并解析为 Vec2
            std::string noPositionStr = obj.asValueMap()["no_position"].asString();
            CCLOG("noPositionStr: %s", noPositionStr.c_str());
            Vec2 noPosition;
            std::istringstream noPosStream(noPositionStr);
            noPosStream >> noPosition.x >> noPosition.y; // 使用空格解析出两个坐标值
            CCLOG("before No Position: x = %.2f, y = %.2f", noPosition.x, noPosition.y);
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX_no = othermapOriginX + noPosition.x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY_no = othermapOriginY + noPosition.y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移
            noPosition.x = adjustedX_no;
            noPosition.y = adjustedY_no;
            // 输出拒绝传送后的位置调试信息
            CCLOG("actual No Position: x = %.2f, y = %.2f", noPosition.x, noPosition.y);

            // 存储到 sceneSwitchPoints
            sceneSwitchPoints.push_back({ switchPos, targetMap, noPosition });
        }
    }


    /////////////////////////////
// 添加小地图
/////////////////////////////
    auto miniMap = Node::create(); // 创建小地图节点
    if (miniMap == nullptr)
    {
        CCLOG("Failed to create minimap");
    }

    float miniMapScale = 0.45f;     // 小地图缩放比例，根据实际地图大小调整
    float miniMapRadius = 100.0f; // 小地图圆形边框半径
    Vec2 miniMapCenter = Vec2(miniMapRadius + 10, visibleSize.height - miniMapRadius - 10); // 小地图中心位置

    // 创建裁剪节点用于实现圆形遮罩
    auto clipper = ClippingNode::create();
    if (clipper == nullptr)
    {
        CCLOG("Failed to create clipper");
    }
    clipper->setPosition(Vec2::ZERO); // 设置裁剪节点位置
    clipper->setAlphaThreshold(0.5f); // 不透明部分保留，透明部分裁剪

    // 创建圆形遮罩
    auto stencil = DrawNode::create();
    if (stencil == nullptr)
    {
        CCLOG("Failed to create stencil");
    }
    stencil->drawSolidCircle(miniMapCenter, miniMapRadius, CC_DEGREES_TO_RADIANS(360), 100, Color4F::WHITE);
    clipper->setStencil(stencil);

    // 缩小主地图并添加到裁剪节点中
    auto miniMapBackground = TMXTiledMap::create(file);
    if (miniMapBackground == nullptr)
    {
        CCLOG("Failed to create minimap background");
    }
    miniMapBackground->setScale(othermap->getScale() * miniMapScale);
    miniMapBackground->setAnchorPoint(Vec2(0, 0));
    miniMapBackground->setPosition(Vec2(miniMapCenter.x - miniMapBackground->getContentSize().width * miniMapScale / 2,
        miniMapCenter.y - miniMapBackground->getContentSize().height * miniMapScale / 2));
    clipper->addChild(miniMapBackground);

    this->addChild(clipper, 4); // 添加裁剪节点到场景中

    // 绘制小地图边框
    auto miniMapBorder = DrawNode::create();
    if (miniMapBorder == nullptr)
    {
        CCLOG("Failed to create minimap border");
    }
    miniMapBorder->drawCircle(miniMapCenter, miniMapRadius, CC_DEGREES_TO_RADIANS(360), 100, false, Color4F::WHITE);
    miniMapBorder->drawSolidCircle(miniMapCenter, miniMapRadius, CC_DEGREES_TO_RADIANS(360), 100, Color4F(0, 0, 0, 1)); // 半透明背景
    this->addChild(miniMapBorder, 3); // 添加边框到场景中

    // 同步主地图角色到小地图
    auto heroMarker = DrawNode::create(); // 英雄位置标记
    if (heroMarker == nullptr)
    {
        CCLOG("Failed to create hero marker");
    }
    heroMarker->drawDot(Vec2::ZERO, 3.5f, Color4F::GREEN);
    clipper->addChild(heroMarker,7);

    auto demonMarker = DrawNode::create(); // 敌人位置标记
    if (demonMarker == nullptr)
    {
        CCLOG("Failed to create demon marker");
    }
    demonMarker->drawDot(Vec2::ZERO, 3.5f, Color4F::RED);
    clipper->addChild(demonMarker,7);

    /////////////////////////////
    // 更新函数：同步小地图上的角色位置和视图区域
    /////////////////////////////
    this->schedule([this, miniMapScale, miniMapCenter, miniMapBackground, heroMarker, demonMarker](float dt) {
        // 获取主地图上的英雄和敌人
        auto hero = dynamic_cast<Character*>(this->getChildByName("hero"));
       // auto demon = dynamic_cast<Enemy*>(this->getChildByName("demon"));
        Vec2 offset, miniHeroPos, miniDemonPos;
        if (hero) {
            // 英雄在主地图中的位置
            Vec2 heroPos = hero->getPosition();

            // 计算小地图英雄位置
            miniHeroPos = (heroPos - othermap->getPosition() + Vec2(othermap->getContentSize().width / 2, othermap->getContentSize().height / 2)) * miniMapScale;
            heroMarker->setPosition(miniMapCenter - miniHeroPos + heroPos * miniMapScale);

            // 更新小地图视图位置，使英雄保持在小地图中央
            offset = miniMapCenter - miniHeroPos;
            miniMapBackground->setPosition(offset);


        }

     /*   if (demon) {
            // 敌人在主地图中的位置
            Vec2 demonPos = demon->getPosition();

            // 计算小地图敌人位置
            miniDemonPos = (demonPos - othermap->getPosition() + Vec2(othermap->getContentSize().width / 2, othermap->getContentSize().height / 2)) * miniMapScale;
            demonMarker->setPosition(miniMapCenter - miniHeroPos + demonPos * miniMapScale);
        }

       */


        }, "update_minimap_key");



    // 添加键盘事件监听器
    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(OtherScene::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(OtherScene::onKeyReleased, this);

    // 将监听器添加到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // 注册 update 函数，让它每一帧都被调用
    this->schedule([this](float dt) {
        this->update(dt);
        }, "update_key");

    return true;
}

void OtherScene::update(float dt)
{
    Node::update(dt);


    auto children = getChildren();
    for (auto child : children) {
        auto character = dynamic_cast<Character*>(child);
        if (character) {
            character->update(dt);
        }
    }
    //遍历每一个传送点信息
    auto hero = dynamic_cast<Character*>(this->getChildByName("hero"));
    if (hero) {
        for (auto& switchPoint : sceneSwitchPoints) {
            //判断地图传送点和人物是否碰撞
         // 跳过未激活的传送点
            if (!switchPoint.isActive) continue;

            // 判断是否有弹窗正在显示
            if (!isDialogActive && hero->getBoundingBox().intersectsRect(Rect(switchPoint.position.x - 10, switchPoint.position.y - 10, 20, 20))) {
                isDialogActive = true;

                auto dialog = LayerColor::create(Color4B(0, 0, 0, 128));
                this->addChild(dialog, 10);

                auto label = Label::createWithSystemFont("Do you want to go to the " + switchPoint.targetMap, "Arial", 40);
                label->setPosition(Director::getInstance()->getVisibleSize() / 2 + Size(0, 50));
                dialog->addChild(label);

                auto yesButton = cocos2d::ui::Button::create();
                yesButton->setTitleText("Yes");
                yesButton->setTitleFontSize(30);
                yesButton->setPosition(Director::getInstance()->getVisibleSize() / 2 + Size(-50, -20));
                dialog->addChild(yesButton);

                auto noButton = cocos2d::ui::Button::create();
                noButton->setTitleText("No");
                noButton->setTitleFontSize(30);
                noButton->setPosition(Director::getInstance()->getVisibleSize() / 2 + Size(50, -20));
                dialog->addChild(noButton);

                // Yes 按钮的回调
                yesButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    CCLOG("User selected YES. Teleporting to %s.", switchPoint.targetMap.c_str());
                    CCLOG("actual Switch Position: x = %.2f, y = %.2f", switchPoint.position.x, switchPoint.position.y);

                    dialog->removeFromParent();
                    isDialogActive = false; // 恢复标志位
                    if (switchPoint.targetMap == "mainmap.tmx") {
                        Director::getInstance()->pushScene(MainScene::createScene());
                    }
                    });

                // No 按钮的回调
                noButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    CCLOG("User selected NO. Dialog removed.");
                    if (dialog) {
                        dialog->removeFromParent();
                        // 恢复标志位
                        isDialogActive = false;
                    }

                    // 将角色移出传送点范围
                    //float offset = 100.0f;
                    hero->setPosition(switchPoint.no_position.x , switchPoint.no_position.y);
                    CCLOG("Hero moved out of teleport area to position (%.2f, %.2f)", hero->getPositionX(), hero->getPositionY());

                    });
            }
        }
    }
}

void OtherScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
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

void OtherScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
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

void OtherScene::returnToLastScene()
{
    // 使用 popScene 返回到之前的场景
    Director::getInstance()->popScene();
}

void OtherScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
