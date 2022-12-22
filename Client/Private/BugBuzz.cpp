#include "stdafx.h"
#include "..\Public\BugBuzz.h"

#include "GameInstance.h"

CBugBuzz::CBugBuzz(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBugBuzz::CBugBuzz(const CBugBuzz & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBugBuzz::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBugBuzz::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("벌레의 야단법석");
	m_SkillInfo.strInfo = TEXT("진동으로 음파를 일으켜서 공격한다.");
	m_SkillInfo.iSkillNum = 4;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 90;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 10;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = BUG;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CBugBuzz::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (m_fSkillTime > 0.1f)
		{
			Set_Pos(fTimeDelta);
			m_fSkillTime = 0.f;
			if (!m_bSkill)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BugBuzz1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
					return;

				RELEASE_INSTANCE(CGameInstance);
				m_bSkill = true;
			}
		}
		m_fDeadTime += fTimeDelta;
		if (m_fDeadTime > 0.6f)
		{
			if (!m_bHitSkill)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BugBuzz3"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
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

void CBugBuzz::Late_Tick(_float fTimeDelta)
{

}

HRESULT CBugBuzz::Render()
{
	return S_OK;
}

HRESULT CBugBuzz::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

void CBugBuzz::Set_Pos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	for (_int i = 0; i < 5; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BugBuzz2"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
			return;
	}
	RELEASE_INSTANCE(CGameInstance);
}

CBugBuzz * CBugBuzz::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBugBuzz*	pInstance = new CBugBuzz(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBugBuzz"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBugBuzz::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBugBuzz(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBugBuzz"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBugBuzz::Free()
{
	__super::Free();

}
