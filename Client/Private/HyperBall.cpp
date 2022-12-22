#include "stdafx.h"
#include "..\Public\HyperBall.h"

#include "GameInstance.h"

CHyperBall::CHyperBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CHyperBall::CHyperBall(const CHyperBall & rhs)
	: CGameObj(rhs)
{
}

HRESULT CHyperBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHyperBall::Initialize(void * pArg)
{


	m_ItemInfo.strName = TEXT("하이퍼볼");
	m_ItemInfo.strInfo = TEXT("높은 확률로 포켓몬을 잡을 수 있다.");
	m_ItemInfo.iItemNum = 2;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 200;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CHyperBall::Tick(_float fTimeDelta)
{

}

void CHyperBall::Late_Tick(_float fTimeDelta)
{

}

HRESULT CHyperBall::Render()
{
	return S_OK;
}

HRESULT CHyperBall::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CHyperBall * CHyperBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHyperBall*	pInstance = new CHyperBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHyperBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CHyperBall::Clone(void * pArg)
{
	CGameObject*	pInstance = new CHyperBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHyperBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHyperBall::Free()
{
	__super::Free();

}
