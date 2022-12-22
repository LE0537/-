#include "stdafx.h"
#include "..\Public\IceBadge.h"

#include "GameInstance.h"

CIceBadge::CIceBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CIceBadge::CIceBadge(const CIceBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CIceBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIceBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("얼음의배지");
	m_ItemInfo.strInfo = TEXT("관장포플러의 배지.");
	m_ItemInfo.iItemNum = 48;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CIceBadge::Tick(_float fTimeDelta)
{

}

void CIceBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CIceBadge::Render()
{
	return S_OK;
}

HRESULT CIceBadge::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CIceBadge * CIceBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CIceBadge*	pInstance = new CIceBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CIceBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CIceBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CIceBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CIceBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIceBadge::Free()
{
	__super::Free();

}
