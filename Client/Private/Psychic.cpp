#include "stdafx.h"
#include "..\Public\Psychic.h"

#include "GameInstance.h"

CPsychic::CPsychic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPsychic::CPsychic(const CPsychic & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPsychic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPsychic::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("사이코키네시스");
	m_SkillInfo.strInfo = TEXT("강한 염동력을 상대에게 보내어 공격한다.");
	m_SkillInfo.iSkillNum = 8;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 90;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 10;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = ESPER;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CPsychic::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (!m_bSkill && m_fSkillTime > 0.7f)
		{
			Set_Pos(fTimeDelta);
			m_bSkill = true;
		}

		if (m_bSkill)
		{
			m_fDeadTime = 0.f;
			m_fSkillTime = 0.f;
			m_SkillInfo.bUseSkill = false;
			m_bSkill = false;
			m_bHitSkill = false;
		}

	}
}

void CPsychic::Late_Tick(_float fTimeDelta)
{

}

HRESULT CPsychic::Render()
{
	return S_OK;
}

void CPsychic::Set_Pos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Psychic1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

CPsychic * CPsychic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPsychic*	pInstance = new CPsychic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPsychic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPsychic::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPsychic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPsychic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPsychic::Free()
{
	__super::Free();

}
