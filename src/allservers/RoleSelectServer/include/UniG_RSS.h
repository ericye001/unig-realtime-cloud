/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_RSS.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: RSS½ø³Ì

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 


#ifndef _UNIG_RSS_H__
#define _UNIG_RSS_H__

class UniG_RSS
{
public:
    ~UniG_RSS(void);

    bool Initialize(void);
    void GetILMArgs(ILMArgs& args);
    void MainRun(void);
    void Finalize(void);
    virtual void Update(void);
    void ProcessILMMsg(BaseMessage* pMessage);
    void GetPropertyValue(string strSection, string strPropertyName, int& iValue);
    void GetPropertyValue(string strSection, string strPropertyName, string& strValue);

private:
    UniG_RSS(void);

public:
    static UniG_RSS* Instance(void);
    static UniG_RSS* m_pInstance;

private:
    IniParser* m_pConfig;
};

#endif 
