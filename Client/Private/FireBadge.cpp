#include "stdafx.h"
#include "..\Public\FireBadge.h"

#include "GameInstance.h"

CFireBadge::CFireBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CFireBadge::CFireBadge(const CFireBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CFireBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFireBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("불의배지");
	m_ItemInfo.strInfo = TEXT("관장순무의 배지다.");
	m_ItemInfo.iItemNum = 43;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CFireBadge::Tick(_float fTimeDelta)
{

}

void CFireBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CFireBadge::Render()
{
	return S_OK;
}

HRESULT CFireBadge::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CFireBadge * CFireBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFireBadge*	pInstance = new CFireBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFireBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFireBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CFireBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFireBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireBadge::Free()
{
	__super::Free();

}
