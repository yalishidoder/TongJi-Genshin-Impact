#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__

#include "cocos2d.h"
#include <vector>
#include "Scene/MainScene.h"

class MapManager {
public:
    // 获取 MapManager 的实例（单例模式）
    static MapManager* getInstance();

    // 保存 MainScene 的传送点数据
    void saveMainSceneSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // 保存 forest 的传送点数据
    void saveforestSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // 获取保存的 MainScene 的传送点数据
    std::vector<PositionSwitchPoint> getMainSceneSwitchPoints() const;

    // 获取保存的 forest 的传送点数据
    std::vector<PositionSwitchPoint> getforestSwitchPoints() const;
private:
    // 私有构造函数，防止外部实例化
    MapManager() {}
    ~MapManager() {}

    // 存储 MainScene 的传送点数据
    std::vector<PositionSwitchPoint> mainSceneSwitchPoints;
    std::vector<PositionSwitchPoint> forestSwitchPoints;
};

#endif // __MAPMANAGER_H__