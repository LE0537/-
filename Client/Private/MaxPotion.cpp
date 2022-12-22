#include "stdafx.h"
#include "..\Public\MaxPotion.h"

#include "GameInstance.h"

CMaxPotion::CMaxPotion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CMaxPotion::CMaxPotion(const CMaxPotion & rhs)
	: CGameObj(rhs)
{
}

HRESULT CMaxPotion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMaxPotion::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("기력의 덩어리");
	m_ItemInfo.strInfo = TEXT("포켓몬을 완전 회복시킨다.\n기절한 포켓몬도 회복시킨다.");
	m_ItemInfo.iItemNum = 11;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CMaxPotion::Tick(_float fTimeDelta)
{

}

void CMaxPotion::Late_Tick(_float fTimeDelta)
{

}

HRESULT CMaxPotion::Render()
{
	return S_OK;
}

HRESULT CMaxPotion::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CMaxPotion * CMaxPotion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMaxPotion*	pInstance = new CMaxPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMaxPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMaxPotion::Clone(void * pArg)
{
	CGameObject*	pInstance = new CMaxPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMaxPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaxPotion::Free()
{
	__super::Free();

}
