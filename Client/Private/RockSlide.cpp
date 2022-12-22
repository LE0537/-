#include "stdafx.h"
#include "..\Public\RockSlide.h"

#include "GameInstance.h"

CRockSlide::CRockSlide(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CRockSlide::CRockSlide(const CRockSlide & rhs)
	: CGameObj(rhs)
{
}

HRESULT CRockSlide::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRockSlide::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("스톤 샤워");
	m_SkillInfo.strInfo = TEXT("큰 바위를 세차게 부딪혀서 공격한다.");
	m_SkillInfo.iSkillNum = 9;
	m_SkillInfo.iDmg = 75;
	m_SkillInfo.iSDmg = 0;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 10;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = ROCK;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CRockSlide::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (!m_bSkill && m_fSkillTime > 0.5f)
		{
			m_bSkill = true;
			m_fSkillTime = 0.f;
		}
		if (m_bSkill && m_fSkillTime > 0.05f)
		{
			Set_Pos(fTimeDelta);
			m_fSkillTime = 0.f;
		}
		m_fDeadTime += fTimeDelta;
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

void CRockSlide::Late_Tick(_float fTimeDelta)
{

}

HRESULT CRockSlide::Render()
{
	return S_OK;
}

HRESULT CRockSlide::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

void CRockSlide::Set_Pos(_float fTimeDelta)
{
	for (_int i = 0; i < 3; ++i)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RockSlide1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
	}
}

CRockSlide * CRockSlide::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRockSlide*	pInstance = new CRockSlide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CRockSlide"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CRockSlide::Clone(void * pArg)
{
	CGameObject*	pInstance = new CRockSlide(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CRockSlide"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRockSlide::Free()
{
	__super::Free();

}
