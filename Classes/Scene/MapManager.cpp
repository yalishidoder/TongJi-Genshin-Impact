#include "MapManager.h"

// 获取 MapManager 的单例
MapManager* MapManager::getInstance() {
    static MapManager instance;
    return &instance;
}

// 保存 MainScene 的传送点数据
void MapManager::saveMainSceneSwitchPoints(const std::vector<PositionSwitchPoint>& points) {
    mainSceneSwitchPoints = points;
}

void MapManager::saveforestSwitchPoints(const std::vector<PositionSwitchPoint>& points)
{
    forestSwitchPoints = points;
}

void MapManager::savedesertSwitchPoints(const std::vector<PositionSwitchPoint>& points)
{
    desertSwitchPoints = points;
}

// 获取保存的 MainScene 的传送点数据
std::vector<PositionSwitchPoint> MapManager::getMainSceneSwitchPoints() const {
    return mainSceneSwitchPoints;
}

std::vector<PositionSwitchPoint> MapManager::getforestSwitchPoints() const
{
    return forestSwitchPoints;
}

std::vector<PositionSwitchPoint> MapManager::getdesertSwitchPoints() const
{
    return desertSwitchPoints;
}
