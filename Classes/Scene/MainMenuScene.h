/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MainMenuScene.h
 * File Function: ��¼ҳ���ʵ�ֵ�������
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12.9
 ****************************************************************/

#ifndef __MAIN_MENU_SCENE_H__
#define __MAIN_MENU_SCENE_H__

#include "cocos2d.h"

class MainMenuScene : public cocos2d::Scene
{
public:
	static Scene* createScene();

	virtual bool init();

	void menuItem1Callback(cocos2d::Ref* pSender);
	void menuItem2Callback(cocos2d::Ref* pSender);

	// �ֶ�ʵ�֡�static create()������
	CREATE_FUNC(MainMenuScene);

	~MainMenuScene();
};

#endif // __MAINMENUSCENE_H__