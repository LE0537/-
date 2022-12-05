#include "stdafx.h"
#include "..\Public\FullPotion.h"

#include "GameInstance.h"

CFullPotion::CFullPotion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CFullPotion::CFullPotion(const CFullPotion & rhs)
	: CGameObj(rhs)
{
}

HRESULT CFullPotion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFullPotion::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("풀회복약");
	m_ItemInfo.strInfo = TEXT("상처 입은 포켓몬의 HP를 전부 회복한다.");
	m_ItemInfo.iItemNum = 6;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CFullPotion::Tick(_float fTimeDelta)
{

}

void CFullPotion::Late_Tick(_float fTimeDelta)
{

}

HRESULT CFullPotion::Render()
{
	return S_OK;
}

CFullPotion * CFullPotion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFullPotion*	pInstance = new CFullPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFullPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFullPotion::Clone(void * pArg)
{
	CGameObject*	pInstance = new CFullPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFullPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFullPotion::Free()
{
	__super::Free();

}
