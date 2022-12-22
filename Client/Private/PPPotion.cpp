#include "stdafx.h"
#include "..\Public\PPPotion.h"

#include "GameInstance.h"

CPPPotion::CPPPotion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPPPotion::CPPPotion(const CPPPotion & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPPPotion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPPPotion::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("PP맥스");
	m_ItemInfo.strInfo = TEXT("기술포인트를 전부 회복한다.");
	m_ItemInfo.iItemNum = 12;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CPPPotion::Tick(_float fTimeDelta)
{

}

void CPPPotion::Late_Tick(_float fTimeDelta)
{

}

HRESULT CPPPotion::Render()
{
	return S_OK;
}

HRESULT CPPPotion::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CPPPotion * CPPPotion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPPPotion*	pInstance = new CPPPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPPPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPPPotion::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPPPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPPPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPPPotion::Free()
{
	__super::Free();

}
