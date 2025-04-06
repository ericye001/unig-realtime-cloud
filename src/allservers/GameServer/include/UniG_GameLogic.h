#ifndef __UNIG_GAME_LOGIC_H__
#define __UNIG_GAME_LOGIC_H__

class UniG_GameLogic
{
public:
    virtual ~UniG_GameLogic(){};
    virtual bool Initialize();                  ///< ִ����Ϸ�߼���صĳ�ʼ��
    virtual void Update();                      ///<��Ϸ�߼�������ڣ�������Ҫ���ø���˺�������������ͼ�ĵĸ���
    UniG_PlayerMgr* GetPlayerMgr(){return m_pPlayerMgr;}  ///������ҹ�����

    virtual UniG_GameObject* AllocObject(int nType);

protected:
    UniG_PlayerMgr* m_pPlayerMgr;
};

#endif

