#include "stdafx.h"
#include "..\Public\None.h"
#include "GameInstance.h"

CNone::CNone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CNone::CNone(const CNone & rhs)
	: CGameObj(rhs)
{
}

HRESULT CNone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNone::Initialize(void * pArg)
{
//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("¾øÀ½");
	m_ItemInfo.strInfo = TEXT("");
	m_ItemInfo.iItemNum = 99;
	m_ItemInfo.iNum = 0;
	m_ItemInfo.iPrice = 0;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CNone::Tick(_float fTimeDelta)
{
}

void CNone::Late_Tick(_float fTimeDelta)
{
}

HRESULT CNone::Render()
{

	return S_OK;
}

HRESULT CNone::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CNone * CNone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNone*	pInstance = new CNone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CNone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CNone::Clone(void * pArg)
{
	CGameObject*	pInstance = new CNone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CNone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNone::Free()
{
	__super::Free();

}
