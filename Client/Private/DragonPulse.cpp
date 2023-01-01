#include "stdafx.h"
#include "..\Public\DragonPulse.h"

#include "GameInstance.h"
#include "SoundMgr.h"

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
	m_SkillInfo.strName = TEXT("�����ĵ�");
	m_SkillInfo.strInfo = TEXT("ū ������ ����ĸ� �����Ѽ� ��븦 �����Ѵ�.");
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
		if (!m_bSound)
		{
			CSoundMgr::Get_Instance()->PlayEffect(TEXT("DragonPulse.mp3"), 1.f);
			m_bSound = true;
		}
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
			m_bSound = false;
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

HRESULT CDragonPulse::Render_ShadowDepth()
{
	return E_NOTIMPL;
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
