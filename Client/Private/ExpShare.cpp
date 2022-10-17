#include "stdafx.h"
#include "..\Public\ExpShare.h"

#include "GameInstance.h"

CExpShare::CExpShare(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CExpShare::CExpShare(const CExpShare & rhs)
	: CGameObj(rhs)
{
}

HRESULT CExpShare::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExpShare::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("학습장치");
	m_ItemInfo.strInfo = TEXT("지니고 있으면 전투를 하지 않아도 경험치를 얻을 수 있다.");
	m_ItemInfo.iItemNum = 30;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 5000;


	return S_OK;
}

void CExpShare::Tick(_float fTimeDelta)
{

}

void CExpShare::Late_Tick(_float fTimeDelta)
{

}

HRESULT CExpShare::Render()
{
	return S_OK;
}

CExpShare * CExpShare::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExpShare*	pInstance = new CExpShare(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CExpShare"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CExpShare::Clone(void * pArg)
{
	CGameObject*	pInstance = new CExpShare(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CExpShare"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpShare::Free()
{
	__super::Free();

}
