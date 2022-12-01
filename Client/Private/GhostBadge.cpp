#include "stdafx.h"
#include "..\Public\GhostBadge.h"

#include "GameInstance.h"

CGhostBadge::CGhostBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CGhostBadge::CGhostBadge(const CGhostBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CGhostBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGhostBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("유령의배지");
	m_ItemInfo.strInfo = TEXT("관장어니언의 배지다.");
	m_ItemInfo.iItemNum = 45;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CGhostBadge::Tick(_float fTimeDelta)
{

}

void CGhostBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CGhostBadge::Render()
{
	return S_OK;
}

CGhostBadge * CGhostBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGhostBadge*	pInstance = new CGhostBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CGhostBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CGhostBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CGhostBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CGhostBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGhostBadge::Free()
{
	__super::Free();

}
