#include "stdafx.h"
#include "..\Public\Candy.h"

#include "GameInstance.h"

CCandy::CCandy(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CCandy::CCandy(const CCandy & rhs)
	: CGameObj(rhs)
{
}

HRESULT CCandy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCandy::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("이상한 사탕");
	m_ItemInfo.strInfo = TEXT("포켓몬의 레벨을 1 올린다.");
	m_ItemInfo.iItemNum = 13;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CCandy::Tick(_float fTimeDelta)
{

}

void CCandy::Late_Tick(_float fTimeDelta)
{

}

HRESULT CCandy::Render()
{
	return S_OK;
}

HRESULT CCandy::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CCandy * CCandy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCandy*	pInstance = new CCandy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCandy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCandy::Clone(void * pArg)
{
	CGameObject*	pInstance = new CCandy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCandy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCandy::Free()
{
	__super::Free();

}
