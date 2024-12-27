# TongJi-Genshin-Impact
同济大学2024年程设期末项目

基于cocos-2dx 3.17.2实现的类原神游戏项目，但画面由3d变为2d

## 项目简介
[请参见原神官方网站](https://ys.mihoyo.com/main/)
## 成员分工

| 姓名 | 学号 | 分工 | 占比 |
| :---: | :---: | :---: | :---: |
| 韦世贸（组长）|2351131|角色面板建立<br/>敌人的攻击逻辑以及敌人的管理<br/>元素反应逻辑<br/>项目调试|25%|
| 康凤轩|2350283|敌人与玩家的动画制作<br/>敌人类与玩家类的框架建立<br/>玩家的攻击逻辑与各种技能<br/>玩家存档<br/>项目调试|25%|
| 陈子昂|2352038|地图建立与管理<br/>任务奖励设置<br/>音效设置<br/>项目调试|25%|
| 叶贤伟|2354274|主界面制作<br/>小地图建立<br/>地图迷雾建立<br/>任务设置<br/>项目调试|25%|

## 项目日志

### 2024/12/1<br/>
明确了分工，并决定使用cocos-2dx引擎


### 2024/12/6<br/>
完成主地图模块代码         ——陈子昂


### 2024/12/9<br/>
完成启动界面代码，并整理的文档           ——叶贤伟


### 2024/12/10<br/>
实现地图切换功能           ——陈子昂

实现地图、人物类的大致框架   ——康凤轩


### 2024/12/11<br/>
将所有小地图用一个类实现      ——陈子昂


### 2024/12/12<br/>
更新森林地图

实现敌人追逐与巡逻功能


### 2024/12/13<br/>
添加小地图


### 2024/12/14<br/>
实现地图内的传送锚点


### 2024/12/15<br/>
实现人物碰撞的判断

添加角色动画


### 2024/12/17<br/>
实现玩家攻击功能

添加元素反应基本逻辑

修复弹窗时角色可以操控的bug

更新小地图功能，实现动态放大与缩小

实现传送点存档功能


### 2024/12/18<br/>
修复小地图bug

初步实现玩家面板


### 2024/12/19<br/>
添加武器切换与技能

完善玩家面板ui,增加角色切换按钮


### 2024/12/20<br/>
实现任务交接点与任务完成动画

修复角色切换动画问题，增加敌人死亡动画

添加森林和沙漠任务

实现角色存档


### 2024/12/21<br/>
添加元素反应标签与冰冻效果

添加城镇地图

玩家x技能实现，优化敌人受伤效果


### 2024/12/22<br/>
添加城镇地图任务

添加地图迷雾

增加敌人血条显示与近战攻击功能

实现角色返回出生点恢复生命值的功能

添加敌人命中与死亡音效

小地图同步迷雾

实现完整音效

更新敌人远程攻击逻辑

修复沙漠与城镇碰撞的bug

建立敌人管理器，用于刷新敌人

修复敌人管理器bug

修复x技能bug,更新武器切换逻辑


### 2024/12/23<br/>
修复小地图无法显示玩家面板的bug

添加死亡玩家死亡逻辑与效果

在城镇地图禁用敌人管理器

增加完成任务的奖励

修复音效bug

增加玩家重生功能

增加金币

增加玩家重置功能，修复玩家重生bug



