#include "UniG.h"

UniG_Role::UniG_Role(void)
{
    m_nType = UNIG_TYPE_ROLE;
	Initialize();
}

UniG_Role::~UniG_Role(void)
{
}

bool UniG_Role::Initialize()
{
    return true;
}


void UniG_Role::Finalize()
{

}

//消息处理
void UniG_Role::OnMessage(ObjectMessage* pMsg) 
{
    
}

void UniG_Role::OnUpdate()
{

}

UniG_SubMap* UniG_Role::GetSubMap()
{
	return m_pSubMap;
}
