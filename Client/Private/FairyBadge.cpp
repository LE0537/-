#include "stdafx.h"
#include "..\Public\FairyBadge.h"

#include "GameInstance.h"

CFairyBadge::CFairyBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CFairyBadge::CFairyBadge(const CFairyBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CFairyBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFairyBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("요정의배지");
	m_ItemInfo.strInfo = TEXT("관장포플러의 배지다.");
	m_ItemInfo.iItemNum = 46;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CFairyBadge::Tick(_float fTimeDelta)
{

}

void CFairyBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CFairyBadge::Render()
{
	return S_OK;
}

CFairyBadge * CFairyBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFairyBadge*	pInstance = new CFairyBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFairyBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFairyBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CFairyBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFairyBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFairyBadge::Free()
{
	__super::Free();

}
