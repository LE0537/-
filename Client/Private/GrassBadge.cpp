#include "stdafx.h"
#include "..\Public\GrassBadge.h"

#include "GameInstance.h"

CGrassBadge::CGrassBadge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CGrassBadge::CGrassBadge(const CGrassBadge & rhs)
	: CGameObj(rhs)
{
}

HRESULT CGrassBadge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrassBadge::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("풀의배지");
	m_ItemInfo.strInfo = TEXT("관장아킬의 배지다.");
	m_ItemInfo.iItemNum = 41;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 300;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CGrassBadge::Tick(_float fTimeDelta)
{

}

void CGrassBadge::Late_Tick(_float fTimeDelta)
{

}

HRESULT CGrassBadge::Render()
{
	return S_OK;
}

HRESULT CGrassBadge::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CGrassBadge * CGrassBadge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGrassBadge*	pInstance = new CGrassBadge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CGrassBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CGrassBadge::Clone(void * pArg)
{
	CGameObject*	pInstance = new CGrassBadge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CGrassBadge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrassBadge::Free()
{
	__super::Free();

}
