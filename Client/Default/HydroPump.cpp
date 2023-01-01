#include "stdafx.h"
#include "HydroPump.h"

#include "GameInstance.h"
#include "SoundMgr.h"

CHydroPump::CHydroPump(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CHydroPump::CHydroPump(const CHydroPump & rhs)
	: CGameObj(rhs)
{
}

HRESULT CHydroPump::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHydroPump::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("하이드로펌프");
	m_SkillInfo.strInfo = TEXT("대량의 물을 세찬 기세로 \n상대에게 발사하여 공격한다.");
	m_SkillInfo.iSkillNum = 3;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 110;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 5;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = WATER;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CHydroPump::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		if (!m_bSound)
		{
			CSoundMgr::Get_Instance()->PlayEffect(TEXT("HydroPump.mp3"), 1.f);
			m_bSound = true;
		}

		m_fSkillTime += fTimeDelta;
		if (m_fSkillTime > 0.1f)
		{
			Set_Pos(fTimeDelta);
			m_fSkillTime = 0.f;
			if (!m_bSkill)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HydroPump1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
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

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HydroPump3"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
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
			m_bSound = false;
		}

	}
}

void CHydroPump::Late_Tick(_float fTimeDelta)
{

}

HRESULT CHydroPump::Render()
{
	return S_OK;
}

HRESULT CHydroPump::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

void CHydroPump::Set_Pos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	for (_int i = 0; i < 15; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HydroPump2"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
			return;
	}
	RELEASE_INSTANCE(CGameInstance);
}

CHydroPump * CHydroPump::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHydroPump*	pInstance = new CHydroPump(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHydroPump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CHydroPump::Clone(void * pArg)
{
	CGameObject*	pInstance = new CHydroPump(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHydroPump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHydroPump::Free()
{
	__super::Free();

}
