#include "stdafx.h"
#include "..\Public\WaterBeam.h"

#include "GameInstance.h"
#include "SoundMgr.h"

CWaterBeam::CWaterBeam(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWaterBeam::CWaterBeam(const CWaterBeam & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWaterBeam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterBeam::Initialize(void * pArg)
{
	m_SkillInfo.strName = TEXT("거품광선");
	m_SkillInfo.strInfo = TEXT("강력한 물줄기로 상대를 공격한다.");
	m_SkillInfo.iSkillNum = 1;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 60;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 20;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = WATER;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;


	return S_OK;
}

void CWaterBeam::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		if (!m_bSound)
		{
			CSoundMgr::Get_Instance()->PlayEffect(TEXT("WaterBeam.mp3"), 1.f);
			m_bSound = true;
		}
		m_fSkillTime += fTimeDelta;
		if (m_fSkillTime > 0.1f)
		{
			Set_Pos(fTimeDelta);
			m_fSkillTime = 0.f;
		}
		m_fDeadTime += fTimeDelta;
		if (m_fDeadTime > 2.f)
		{
			m_fDeadTime = 0.f;
			m_fSkillTime = 0.f;
			m_SkillInfo.bUseSkill = false;
			m_bSetPos = false;
			m_bSound = false;
		}
	}
}

void CWaterBeam::Late_Tick(_float fTimeDelta)
{

}

HRESULT CWaterBeam::Render()
{

	return S_OK;
}

HRESULT CWaterBeam::Render_ShadowDepth()
{
	return E_NOTIMPL;
}



CWaterBeam * CWaterBeam::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterBeam*	pInstance = new CWaterBeam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWaterBeam"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObj * CWaterBeam::Clone(void * pArg)
{
	CWaterBeam*	pInstance = new CWaterBeam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWaterBeam"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterBeam::Free()
{
	__super::Free();

}

void CWaterBeam::Set_Pos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	for (_int i = 0; i < 10; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WaterBall"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
			return;
	}
	RELEASE_INSTANCE(CGameInstance);
}
