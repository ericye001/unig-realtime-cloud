#include "UniG.h"

bool UniG_GameLogic::Initialize()
{
    // TODO add a default sub_map
    //m_pPlayerMgr = new UniG_PlayerMgr;
    return true;
}

void UniG_GameLogic::Update()
{
    UniG_Map::Instance()->Update();
}


UniG_GameObject* UniG_GameLogic::AllocObject(int nType)
{
    UniG_GameObject* pResult = NULL;
    switch(nType)
    {
    case UNIG_TYPE_OBJECT:
        {
            pResult = new UniG_GameObject;
            break;
        }
        case UNIG_TYPE_ROLE:
        {
            pResult = new UniG_Role;
            break;
        }
        case UNIG_TYPE_PLAYER:
        {
            pResult = new UniG_Player;
			pResult->Initialize();
            break;
        }
        default:
        {
            break;
        }
    }
    return pResult;
}