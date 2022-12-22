#include "stdafx.h"
#include "..\Public\MonsterBall.h"

#include "GameInstance.h"

CMonsterBall::CMonsterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CMonsterBall::CMonsterBall(const CMonsterBall & rhs)
	: CGameObj(rhs)
{
}

HRESULT CMonsterBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterBall::Initialize(void * pArg)
{

//	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("몬스터볼");
	m_ItemInfo.strInfo = TEXT("포켓몬을 잡을 수 있다.");
	m_ItemInfo.iItemNum = 0;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 200;

	*(ITEMINFO**)pArg = &m_ItemInfo;
	return S_OK;
}

void CMonsterBall::Tick(_float fTimeDelta)
{

}

void CMonsterBall::Late_Tick(_float fTimeDelta)
{

}

HRESULT CMonsterBall::Render()
{
	return S_OK;
}

HRESULT CMonsterBall::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CMonsterBall * CMonsterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterBall*	pInstance = new CMonsterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMonsterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonsterBall::Clone(void * pArg)
{
	CGameObject*	pInstance = new CMonsterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMonsterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBall::Free()
{
	__super::Free();
	
}
