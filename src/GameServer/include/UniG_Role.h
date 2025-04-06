/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Role.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 游戏角色的封装

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_ROLE_H__
#define _UNIG_ROLE_H__

/**
 * 游戏角色的基类，比如Player，Monster和NPC都可以继承它
 */
class UniG_Role : public UniG_GameObject
{
public:
    /**
     * 构造函数
     */
    UniG_Role();

    /**
     * 析构函数
     */
    virtual ~UniG_Role();

    /**
     * 初始化操作，比如加载角信息
     */
	virtual bool Initialize();

    /**
     *  销毁角色信息
     */
    virtual void Finalize();

    /**
     * 回调的函数，用户实现它，用于帧更新
     */
    virtual void OnUpdate();

    /**
     * 响应消息的函数，用户实现它来处理自己的消息
     * @param pMsg 带处理的游戏逻辑消息
     */
    virtual void OnMessage(ObjectMessage* pMsg);
    
    UniG_SubMap* GetSubMap();
};

#endif

