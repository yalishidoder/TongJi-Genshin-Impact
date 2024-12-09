/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MainMenuScene.cpp
 * File Function: ��¼ҳ���ʵ��
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12��9
 ****************************************************************/

#include "MainMenuScene.h"
#include "AudioEngine.h"
#include "MainScene.h"

USING_NS_CC;

using namespace cocos2d::experimental; // ʹ�� AudioEngine  �������ռ�

// ���ڴ洢�������ֵ� ID
static int backgroundMusicId = -1;
// ���ڼ�¼���������Ƿ񲥷�
static bool isMusicPlaying = true;

Scene* MainMenuScene::createScene()
{
    return MainMenuScene::create();
}


bool MainMenuScene::init()
{
    // ������
    if (!Scene::init())
    {
        return false;
    }

    // ��ӱ���
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // ���ű������֣�ʹ�� AudioEngine��
    backgroundMusicId = AudioEngine::play2d("Audio/GenshinImpact_BGM.mp3", true, 0.5f); // ѭ�����ţ����� 0.5
    if (backgroundMusicId == AudioEngine::INVALID_AUDIO_ID) {
        CCLOG("Error: Failed to play background music.");
    }
    else {
        CCLOG("Background music started playing with ID: %d", backgroundMusicId);
    }

    // ����ɵ�������
    Director::getInstance()->getTextureCache()->removeTextureForKey("Scene/MainMenuScene/GenshinImpact_BG.png");

    auto background = Sprite::create("Scene/MainMenuScene/GenshinImpact_BG.png"); // �滻Ϊ��ı���ͼƬ
    if (background)
    {
        background->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
        this->addChild(background);
    }
    else
    {
        CCLOG("Error: Could not load background image");
    }

    // ���Logo
    auto logo = Sprite::create("Scene/MainMenuScene/GenshinImpact_Logo.png"); // �滻Ϊ���LogoͼƬ·��
    if (logo)
    {
        // ����logo��С
        logo->setScale(0.5);
        // ����logoλ��
        logo->setPosition(Vec2(
            origin.x + visibleSize.width - logo->getContentSize().width / 3,              // ˮƽ����
            origin.y + visibleSize.height - logo->getContentSize().height / 2 - 20)); // �ඥ��50����
        this->addChild(logo, 1); // ����Logo�㼶Ϊ 1��ȷ������ʾ�ڱ���֮��
    }
    else 
    {
        CCLOG("Error: Could not load logo image");
    }

     // ������������Ϸ����ť�������õ�����ײ�
    auto startGameButton = MenuItemImage::create(
        "Scene/MainMenuScene/GameBegin1.png",
        "Scene/MainMenuScene/GameBegin2.png",
        [](Ref* sender) {
            // ֹͣ��������
            AudioEngine::stop(backgroundMusicId);
            // �л�����Ϸ����
            Director::getInstance()->replaceScene(TransitionFade::create(1.0,   MainScene::createScene()));
        });

    if (startGameButton == nullptr ||
        startGameButton->getContentSize().width <= 0 ||
        startGameButton->getContentSize().height <= 0)
    {
       CCLOG("'CloseNormal.png' and 'CloseSelected.png'");
    }

    else {
        // ���Ű�ť��С
        startGameButton->setScale(0.3);
        // ���ð�ť��λ��
        startGameButton->setPosition(Vec2(
            origin.x + visibleSize.width / 2,  // ˮƽ����
            origin.y + startGameButton->getContentSize().height / 3-13)); // ��ײ��߶�Ϊ��ťһ��
    }

    // �������˳���Ϸ����ť�������õ������Ҳ�
    auto exitButton = MenuItemImage::create(
        "Scene/MainMenuScene/CloseNormal.png",
        "Scene/MainMenuScene/CloseSelected.png",
        [](Ref* sender) 
        {
            // ֹͣ��������
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
        // ���Ű�ť��С
        exitButton->setScale(0.8);
        // ���ð�ť��λ��
        exitButton->setPosition(Vec2(
            origin.x + visibleSize.width - 150, // ���Ҳ���-150����
            origin.y + startGameButton->getContentSize().height / 3 - 15)); // ��ֱ����
    }

    // ��ӱ������ֿ��ذ�ť
    auto musicToggleButton = MenuItemImage::create(
        "Scene/MainMenuScene/BGM_ON.png",    // ���ֿ���ʱ��ͼ��
        "Scene/MainMenuScene/BGM_OFF.png",  // ���ֹر�ʱ��ͼ��
        [](Ref* sender) {
            if (isMusicPlaying) {
                // ����������ڲ��ţ�����ͣ
                AudioEngine::pause(backgroundMusicId);
                isMusicPlaying = false;
                CCLOG("Background music paused.");
            }
            else {
                // �����������ͣ�����������
                AudioEngine::resume(backgroundMusicId);
                isMusicPlaying = true;
                CCLOG("Background music resumed.");
            }
        });

    if (musicToggleButton) {
        // ��С����
        musicToggleButton->setScale(0.05);
        // ����λ��
        musicToggleButton->setPosition(Vec2(
            origin.x + visibleSize.width - 150,  // ���Ҳ���-150����
            origin.y +  170)); // ��ť������
    }

    // �����˵�
    auto menu = Menu::create(startGameButton, exitButton, musicToggleButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    return true;
}

// ��������ʱ�ͷ���Ƶ��Դ
MainMenuScene::~MainMenuScene()
{
    AudioEngine::stopAll();  // ֹͣ������Ƶ
    AudioEngine::end();      // �ͷ�������Ƶ��Դ
}

