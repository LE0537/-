#include "stdafx.h"
#include "..\Public\WaterBadge.h"

#include "GameInstance.h"

CWaterBadge::CWaterBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWaterBadge::CWaterBadge(const CWaterBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWaterBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("물의배지");
	m_ItemInfo.strInfo = TEXT("관장야청의 배지다.");
	m_ItemInfo.iItemNum = 42;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CWaterBadge::Tick(_float fTimeDelta)
{

}

void CWaterBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CWaterBadge::Render()
{
	return S_OK;
}

CWaterBadge * CWaterBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterBadge*	pInstance = new CWaterBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWaterBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWaterBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CWaterBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWaterBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterBadge::Free()
{
	__super::Free();

}
