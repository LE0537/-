#include "stdafx.h"
#include "..\Public\DragonBadge.h"

#include "GameInstance.h"

CDragonBadge::CDragonBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CDragonBadge::CDragonBadge(const CDragonBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CDragonBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDragonBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("용의배지");
	m_ItemInfo.strInfo = TEXT("관장금랑의 배지다.");
	m_ItemInfo.iItemNum = 50;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CDragonBadge::Tick(_float fTimeDelta)
{

}

void CDragonBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CDragonBadge::Render()
{
	return S_OK;
}

CDragonBadge * CDragonBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDragonBadge*	pInstance = new CDragonBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDragonBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CDragonBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CDragonBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDragonBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragonBadge::Free()
{
	__super::Free();

}
