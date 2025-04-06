/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_GameServer.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: game server process class

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 


#ifndef _UNIG_GAMESERVER_H__
#define _UNIG_GAMESERVER_H__

class UniG_GameServer
{
public:
        // runtime time info
    static uint64  m_nLastFrameTime;
    static uint64  m_nCurFrameTime;
    static uint32  m_nElapsedTime;

	static UniG_GameServer& Instance();
	virtual ~UniG_GameServer(void);
public:
	bool Initialize(void);
	virtual void InitGameLogic(UniG_GameLogic* pGameLogic);

	void GetILMArgs(ILMArgs& args);
	void MainRun(void);
	void Finalize(void);
	virtual void Update(void);
	void ProcessSeamlessMsg(BaseMessage* pMessage);
	void GetPropertyValue(string strSection, string strPropertyName, int& iValue);
	void GetPropertyValue(string strSection, string strPropertyName, string& strValue);
	UniG_GameLogic* GetGameLogic();

protected:
	IniParser* m_pConfig;
	UniG_GameLogic* m_pGameLogic;

private:
	static UniG_GameServer * m_pInstance;
	UniG_GameServer(void);

};

#endif 
