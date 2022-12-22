#include "stdafx.h"
#include "..\Public\EvilBadge.h"

#include "GameInstance.h"

CEvilBadge::CEvilBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CEvilBadge::CEvilBadge(const CEvilBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CEvilBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEvilBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("악의배지");
	m_ItemInfo.strInfo = TEXT("관장두송의 배지다.");
	m_ItemInfo.iItemNum = 49;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CEvilBadge::Tick(_float fTimeDelta)
{

}

void CEvilBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CEvilBadge::Render()
{
	return S_OK;
}

HRESULT CEvilBadge::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CEvilBadge * CEvilBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEvilBadge*	pInstance = new CEvilBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEvilBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CEvilBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CEvilBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEvilBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvilBadge::Free()
{
	__super::Free();

}
