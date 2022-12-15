#include "stdafx.h"
#include "..\Public\DragonPulse.h"

#include "GameInstance.h"

CDragonPulse::CDragonPulse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CDragonPulse::CDragonPulse(const CDragonPulse & rhs)
	: CGameObj(rhs)
{
}

HRESULT CDragonPulse::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDragonPulse::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("용의파동");
	m_SkillInfo.strInfo = TEXT("큰 입으로 충격파를 일으켜서 상대를 공격한다.");
	m_SkillInfo.iSkillNum = 11;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 85;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 10;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = DRAGON;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CDragonPulse::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (!m_bSkill && m_fSkillTime > 0.5f)
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

void CDragonPulse::Late_Tick(_float fTimeDelta)
{

}

HRESULT CDragonPulse::Render()
{
	return S_OK;
}

void CDragonPulse::Set_Pos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DragonPulse1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

CDragonPulse * CDragonPulse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDragonPulse*	pInstance = new CDragonPulse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDragonPulse"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CDragonPulse::Clone(void * pArg)
{
	CGameObject*	pInstance = new CDragonPulse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDragonPulse"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragonPulse::Free()
{
	__super::Free();

}
