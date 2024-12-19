#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__

#include "cocos2d.h"
#include <vector>
#include "Scene/MainScene.h"

class MapManager {
public:
    // ��ȡ MapManager ��ʵ��������ģʽ��
    static MapManager* getInstance();

    // ���� MainScene �Ĵ��͵�����
    void saveMainSceneSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // ���� forest �Ĵ��͵�����
    void saveforestSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // ���� desert �Ĵ��͵�����
    void savedesertSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // ��ȡ����� MainScene �Ĵ��͵�����
    std::vector<PositionSwitchPoint> getMainSceneSwitchPoints() const;

    // ��ȡ����� forest �Ĵ��͵�����
    std::vector<PositionSwitchPoint> getforestSwitchPoints() const;

    // ��ȡ����� desert �Ĵ��͵�����
    std::vector<PositionSwitchPoint> getdesertSwitchPoints() const;


private:
    // ˽�й��캯������ֹ�ⲿʵ����
    MapManager() {}
    ~MapManager() {}

    // �洢 MainScene �Ĵ��͵�����
    std::vector<PositionSwitchPoint> mainSceneSwitchPoints;
    std::vector<PositionSwitchPoint> forestSwitchPoints;
    std::vector<PositionSwitchPoint> desertSwitchPoints;
};

#endif // __MAPMANAGER_H__