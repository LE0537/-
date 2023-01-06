#include "stdafx.h"
#include "..\Public\Candy2.h"

#include "GameInstance.h"

CCandy2::CCandy2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CCandy2::CCandy2(const CCandy2 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CCandy2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCandy2::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	
	m_ItemInfo.strName = TEXT("잃어버린 구슬");
	m_ItemInfo.strInfo = TEXT("누군가 잃어버린 소중한 구슬.\n포켓몬의 레벨을 10 올린다.");
	m_ItemInfo.iItemNum = 14;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CCandy2::Tick(_float fTimeDelta)
{

}

void CCandy2::Late_Tick(_float fTimeDelta)
{

}

HRESULT CCandy2::Render()
{
	return S_OK;
}

HRESULT CCandy2::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CCandy2 * CCandy2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCandy2*	pInstance = new CCandy2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCandy2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCandy2::Clone(void * pArg)
{
	CGameObject*	pInstance = new CCandy2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCandy2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCandy2::Free()
{
	__super::Free();

}
