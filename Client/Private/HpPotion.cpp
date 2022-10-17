#include "stdafx.h"
#include "..\Public\HpPotion.h"

#include "GameInstance.h"

CHpPotion::CHpPotion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CHpPotion::CHpPotion(const CHpPotion & rhs)
	: CGameObj(rhs)
{
}

HRESULT CHpPotion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHpPotion::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("상처약");
	m_ItemInfo.strInfo = TEXT("상처 입은 포켓몬의 HP를 30 회복한다.");
	m_ItemInfo.iItemNum = 4;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;


	return S_OK;
}

void CHpPotion::Tick(_float fTimeDelta)
{

}

void CHpPotion::Late_Tick(_float fTimeDelta)
{

}

HRESULT CHpPotion::Render()
{
	return S_OK;
}

CHpPotion * CHpPotion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHpPotion*	pInstance = new CHpPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHpPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CHpPotion::Clone(void * pArg)
{
	CGameObject*	pInstance = new CHpPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHpPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpPotion::Free()
{
	__super::Free();

}
