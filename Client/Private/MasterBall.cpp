#include "stdafx.h"
#include "..\Public\MasterBall.h"

#include "GameInstance.h"

CMasterBall::CMasterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CMasterBall::CMasterBall(const CMasterBall & rhs)
	: CGameObj(rhs)
{
}

HRESULT CMasterBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMasterBall::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("마스터볼");
	m_ItemInfo.strInfo = TEXT("포켓몬을 무조건 잡을 수 있다.");
	m_ItemInfo.iItemNum = 3;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 200;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CMasterBall::Tick(_float fTimeDelta)
{

}

void CMasterBall::Late_Tick(_float fTimeDelta)
{

}

HRESULT CMasterBall::Render()
{
	return S_OK;
}

CMasterBall * CMasterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMasterBall*	pInstance = new CMasterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMasterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMasterBall::Clone(void * pArg)
{
	CGameObject*	pInstance = new CMasterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMasterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMasterBall::Free()
{
	__super::Free();

}
