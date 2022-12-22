#include "stdafx.h"
#include "..\Public\FightBadge.h"

#include "GameInstance.h"

CFightBadge::CFightBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CFightBadge::CFightBadge(const CFightBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CFightBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFightBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("격투의배지");
	m_ItemInfo.strInfo = TEXT("관장채두의 배지다.");
	m_ItemInfo.iItemNum = 44;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CFightBadge::Tick(_float fTimeDelta)
{

}

void CFightBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CFightBadge::Render()
{
	return S_OK;
}

HRESULT CFightBadge::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CFightBadge * CFightBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFightBadge*	pInstance = new CFightBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFightBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFightBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CFightBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFightBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFightBadge::Free()
{
	__super::Free();

}
