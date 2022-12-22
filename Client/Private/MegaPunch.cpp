#include "stdafx.h"
#include "..\Public\MegaPunch.h"

#include "GameInstance.h"

CMegaPunch::CMegaPunch(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CMegaPunch::CMegaPunch(const CMegaPunch & rhs)
	: CGameObj(rhs)
{
}

HRESULT CMegaPunch::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMegaPunch::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("¸Þ°¡Åæ ÆÝÄ¡");
	m_SkillInfo.strInfo = TEXT("ÈûÀ» ´ãÀº ÆÝÄ¡·Î »ó´ë¸¦ °ø°ÝÇÑ´Ù.");
	m_SkillInfo.iSkillNum = 7;
	m_SkillInfo.iDmg = 85;
	m_SkillInfo.iSDmg = 0;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 20;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = NORMAL;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CMegaPunch::Tick(_float fTimeDelta)
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

void CMegaPunch::Late_Tick(_float fTimeDelta)
{

}

HRESULT CMegaPunch::Render()
{
	return S_OK;
}

HRESULT CMegaPunch::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

void CMegaPunch::Set_Pos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MegaPunch1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

CMegaPunch * CMegaPunch::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMegaPunch*	pInstance = new CMegaPunch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMegaPunch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMegaPunch::Clone(void * pArg)
{
	CGameObject*	pInstance = new CMegaPunch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMegaPunch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMegaPunch::Free()
{
	__super::Free();

}
