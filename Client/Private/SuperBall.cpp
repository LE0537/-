#include "stdafx.h"
#include "..\Public\SuperBall.h"

#include "GameInstance.h"

CSuperBall::CSuperBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CSuperBall::CSuperBall(const CSuperBall & rhs)
	: CGameObj(rhs)
{
}

HRESULT CSuperBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSuperBall::Initialize(void * pArg)
{

	

	m_ItemInfo.strName = TEXT("슈퍼볼");
	m_ItemInfo.strInfo = TEXT("포켓몬을 잡을 수 있다.\n몬스터볼 보다 확률이 높다.");
	m_ItemInfo.iItemNum = 1;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 200;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CSuperBall::Tick(_float fTimeDelta)
{

}

void CSuperBall::Late_Tick(_float fTimeDelta)
{

}

HRESULT CSuperBall::Render()
{
	return S_OK;
}

HRESULT CSuperBall::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CSuperBall * CSuperBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSuperBall*	pInstance = new CSuperBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSuperBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CSuperBall::Clone(void * pArg)
{
	CGameObject*	pInstance = new CSuperBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSuperBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSuperBall::Free()
{
	__super::Free();

}
