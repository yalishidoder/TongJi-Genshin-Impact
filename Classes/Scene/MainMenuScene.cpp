/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MainMenuScene.cpp
 * File Function: 登录页面的实现
 * Author:        叶贤伟
 * Update Date:   2024.12。9
 ****************************************************************/

#include "MainMenuScene.h"
#include "AudioEngine.h"
#include "MainScene.h"

USING_NS_CC;

using namespace cocos2d::experimental; // 使用 AudioEngine  的命名空间

// 用于存储背景音乐的 ID
static int backgroundMusicId = -1;
// 用于记录背景音乐是否播放
static bool isMusicPlaying = true;

Scene* MainMenuScene::createScene()
{
    return MainMenuScene::create();
}


bool MainMenuScene::init()
{
    // 错误处理
    if (!Scene::init())
    {
        return false;
    }

    // 添加背景
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 播放背景音乐（使用 AudioEngine）
    backgroundMusicId = AudioEngine::play2d("Audio/GenshinImpact_BGM.mp3", true, 0.5f); // 循环播放，音量 0.5
    if (backgroundMusicId == AudioEngine::INVALID_AUDIO_ID) {
        CCLOG("Error: Failed to play background music.");
    }
    else {
        CCLOG("Background music started playing with ID: %d", backgroundMusicId);
    }

    // 清除旧的纹理缓存
    Director::getInstance()->getTextureCache()->removeTextureForKey("Scene/MainMenuScene/GenshinImpact_BG.png");

    auto background = Sprite::create("Scene/MainMenuScene/GenshinImpact_BG.png"); // 替换为你的背景图片
    if (background)
    {
        background->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
        this->addChild(background);
    }
    else
    {
        CCLOG("Error: Could not load background image");
    }

    // 添加Logo
    auto logo = Sprite::create("Scene/MainMenuScene/GenshinImpact_Logo.png"); // 替换为你的Logo图片路径
    if (logo)
    {
        // 设置logo大小
        logo->setScale(0.5);
        // 设置logo位置
        logo->setPosition(Vec2(
            origin.x + visibleSize.width - logo->getContentSize().width / 3,              // 水平居中
            origin.y + visibleSize.height - logo->getContentSize().height / 2 - 20)); // 距顶部50像素
        this->addChild(logo, 1); // 设置Logo层级为 1，确保它显示在背景之上
    }
    else 
    {
        CCLOG("Error: Could not load logo image");
    }

     // 创建“进入游戏”按钮，并设置到画面底部
    auto startGameButton = MenuItemImage::create(
        "Scene/MainMenuScene/GameBegin1.png",
        "Scene/MainMenuScene/GameBegin2.png",
        [](Ref* sender) {
            // 停止背景音乐
            AudioEngine::stop(backgroundMusicId);
            // 切换到游戏场景
            Director::getInstance()->replaceScene(TransitionFade::create(1.0,   MainScene::createScene()));
        });

    if (startGameButton == nullptr ||
        startGameButton->getContentSize().width <= 0 ||
        startGameButton->getContentSize().height <= 0)
    {
       CCLOG("'CloseNormal.png' and 'CloseSelected.png'");
    }

    else {
        // 缩放按钮大小
        startGameButton->setScale(0.3);
        // 设置按钮的位置
        startGameButton->setPosition(Vec2(
            origin.x + visibleSize.width / 2,  // 水平居中
            origin.y + startGameButton->getContentSize().height / 3-13)); // 距底部高度为按钮一半
    }

    // 创建“退出游戏”按钮，并设置到画面右侧
    auto exitButton = MenuItemImage::create(
        "Scene/MainMenuScene/CloseNormal.png",
        "Scene/MainMenuScene/CloseSelected.png",
        [](Ref* sender) 
        {
            // 停止背景音乐
            AudioEngine::stop(backgroundMusicId);

            Director::getInstance()->end();
        });

    if (startGameButton == nullptr ||
        startGameButton->getContentSize().width <= 0 ||
        startGameButton->getContentSize().height <= 0)
    {
        CCLOG("'CloseNormal.png' and 'CloseSelected.png'");
    }

    else 
    {
        // 缩放按钮大小
        exitButton->setScale(0.8);
        // 设置按钮的位置
        exitButton->setPosition(Vec2(
            origin.x + visibleSize.width - 150, // 距右侧宽度-150像素
            origin.y + startGameButton->getContentSize().height / 3 - 15)); // 垂直居中
    }

    // 添加背景音乐开关按钮
    auto musicToggleButton = MenuItemImage::create(
        "Scene/MainMenuScene/BGM_ON.png",    // 音乐开启时的图标
        "Scene/MainMenuScene/BGM_OFF.png",  // 音乐关闭时的图标
        [](Ref* sender) {
            if (isMusicPlaying) {
                // 如果音乐正在播放，则暂停
                AudioEngine::pause(backgroundMusicId);
                isMusicPlaying = false;
                CCLOG("Background music paused.");
            }
            else {
                // 如果音乐已暂停，则继续播放
                AudioEngine::resume(backgroundMusicId);
                isMusicPlaying = true;
                CCLOG("Background music resumed.");
            }
        });

    if (musicToggleButton) {
        // 大小缩放
        musicToggleButton->setScale(0.05);
        // 设置位置
        musicToggleButton->setPosition(Vec2(
            origin.x + visibleSize.width - 150,  // 距右侧宽度-150像素
            origin.y +  170)); // 按钮靠顶部
    }

    // 创建菜单
    auto menu = Menu::create(startGameButton, exitButton, musicToggleButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    return true;
}

// 场景销毁时释放音频资源
MainMenuScene::~MainMenuScene()
{
    AudioEngine::stopAll();  // 停止所有音频
    AudioEngine::end();      // 释放所有音频资源
}

