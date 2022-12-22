#include "stdafx.h"
#include "..\Public\Thunder.h"

#include "GameInstance.h"

CThunder::CThunder(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CThunder::CThunder(const CThunder & rhs)
	: CGameObj(rhs)
{
}

HRESULT CThunder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CThunder::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("번개");
	m_SkillInfo.strInfo = TEXT("강한 번개를 상대에게 떨어뜨려 공격한다. ");
	m_SkillInfo.iSkillNum = 6;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 110;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 10;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = THUNDER;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CThunder::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (!m_bSkill && m_fSkillTime > 0.5f)
		{
			m_bSkill = true;
		}
		if (m_bSkill && m_fSkillTime > 0.1f)
		{
			Set_Pos(fTimeDelta);
			m_fSkillTime = 0.f;
		
		}
		m_fDeadTime += fTimeDelta;
		if (m_fDeadTime > 0.6f)
		{
			if (!m_bHitSkill)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Thunder3"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
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

void CThunder::Late_Tick(_float fTimeDelta)
{

}

HRESULT CThunder::Render()
{
	return S_OK;
}

HRESULT CThunder::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

void CThunder::Set_Pos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	for (_int i = 0; i < 2; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Thunder1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
			return;
	}
	RELEASE_INSTANCE(CGameInstance);
}

CThunder * CThunder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CThunder*	pInstance = new CThunder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CThunder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CThunder::Clone(void * pArg)
{
	CGameObject*	pInstance = new CThunder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CThunder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThunder::Free()
{
	__super::Free();

}
