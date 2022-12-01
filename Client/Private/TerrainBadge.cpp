#include "stdafx.h"
#include "..\Public\TerrainBadge.h"

#include "GameInstance.h"

CTerrainBadge::CTerrainBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CTerrainBadge::CTerrainBadge(const CTerrainBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CTerrainBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrainBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("땅의배지");
	m_ItemInfo.strInfo = TEXT("관장마쿠와의 배지");
	m_ItemInfo.iItemNum = 47;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CTerrainBadge::Tick(_float fTimeDelta)
{

}

void CTerrainBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CTerrainBadge::Render()
{
	return S_OK;
}

CTerrainBadge * CTerrainBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrainBadge*	pInstance = new CTerrainBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTerrainBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTerrainBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CTerrainBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrainBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrainBadge::Free()
{
	__super::Free();

}
