#include "stdafx.h"
#include "..\Public\BodyPress.h"

#include "GameInstance.h"
#include "SoundMgr.h"

CBodyPress::CBodyPress(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBodyPress::CBodyPress(const CBodyPress & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBodyPress::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBodyPress::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("바디프레스");
	m_SkillInfo.strInfo = TEXT("상대를 향해서 몸 전체를 던져 강력한 공격을한다.");
	m_SkillInfo.iSkillNum = 12;
	m_SkillInfo.iDmg = 80;
	m_SkillInfo.iSDmg = 0;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 10;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = FIGHT;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CBodyPress::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (m_fSkillTime > 1.f)
		{
			if (!m_bSound)
			{
				CSoundMgr::Get_Instance()->PlayEffect(TEXT("BodyPress.mp3"), 1.f);
				m_bSound = true;
			}
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tackle2"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
				return;
			for (_int i = 0; i < 30; ++i)
			{

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BodyPress1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
					return;
			}
			RELEASE_INSTANCE(CGameInstance);
			m_fSkillTime = 0.f;
			m_SkillInfo.bUseSkill = false;
			m_bSound = false;
		}
	}
}

void CBodyPress::Late_Tick(_float fTimeDelta)
{

}

HRESULT CBodyPress::Render()
{
	return S_OK;
}

HRESULT CBodyPress::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CBodyPress * CBodyPress::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBodyPress*	pInstance = new CBodyPress(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBodyPress"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBodyPress::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBodyPress(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBodyPress"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBodyPress::Free()
{
	__super::Free();

}
