#ifndef __UNIG_GAME_LOGIC_H__
#define __UNIG_GAME_LOGIC_H__

class UniG_GameLogic
{
public:
    virtual ~UniG_GameLogic(){};
    virtual bool Initialize();                  ///< 执行游戏逻辑相关的初始化
    virtual void Update();                      ///<游戏逻辑处理入口，子类需要调用父类此函数，以驱动地图的的更新
    UniG_PlayerMgr* GetPlayerMgr(){return m_pPlayerMgr;}  ///返回玩家管理者

    virtual UniG_GameObject* AllocObject(int nType);

protected:
    UniG_PlayerMgr* m_pPlayerMgr;
};

#endif

