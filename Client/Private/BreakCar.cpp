#include "stdafx.h"
#include "..\Public\BreakCar.h"

#include "GameInstance.h"

CBreakCar::CBreakCar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBreakCar::CBreakCar(const CBreakCar & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBreakCar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBreakCar::Initialize(void * pArg)
{

	//*(CGameObject**)pArg = this;


	m_SkillInfo.strName = TEXT("난폭운전");
	m_SkillInfo.strInfo = TEXT("배해월 특기 난폭운전 이다.");
	m_SkillInfo.iSkillNum = 98;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 150;
	m_SkillInfo.fHit = 50;
	m_SkillInfo.iMaxPoint = 5;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = EVIL;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CBreakCar::Tick(_float fTimeDelta)
{

}

void CBreakCar::Late_Tick(_float fTimeDelta)
{

}

HRESULT CBreakCar::Render()
{
	return S_OK;
}

CBreakCar * CBreakCar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBreakCar*	pInstance = new CBreakCar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBreakCar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBreakCar::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBreakCar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBreakCar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreakCar::Free()
{
	__super::Free();

}
