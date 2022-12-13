#include "stdafx.h"
#include "..\Public\Bite.h"

#include "GameInstance.h"

CBite::CBite(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBite::CBite(const CBite & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBite::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("깨물어부수기");
	m_SkillInfo.strInfo = TEXT("날카로운 이빨로 상대를 깨물어 부숴서 공격한다.");
	m_SkillInfo.iSkillNum = 5;
	m_SkillInfo.iDmg = 80;
	m_SkillInfo.iSDmg = 0;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 15;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = EVIL;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CBite::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (m_fSkillTime > 0.1f)
		{
			m_fSkillTime = 0.f;
			if (!m_bSkill)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bite1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
					return;

				RELEASE_INSTANCE(CGameInstance);
				m_bSkill = true;
			}
		}
		m_fDeadTime += fTimeDelta;
		if (m_fDeadTime > 1.2f)
		{
			if (!m_bHitSkill)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bite2"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
					return;

				RELEASE_INSTANCE(CGameInstance);
				m_bHitSkill = true;
			}
		}
		if (m_fDeadTime > 1.5f)
		{
			m_fDeadTime = 0.f;
			m_fSkillTime = 0.f;
			m_SkillInfo.bUseSkill = false;
			m_bSkill = false;
			m_bHitSkill = false;
		}

	}
}

void CBite::Late_Tick(_float fTimeDelta)
{

}

HRESULT CBite::Render()
{
	return S_OK;
}

CBite * CBite::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBite*	pInstance = new CBite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBite::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBite(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBite::Free()
{
	__super::Free();

}
