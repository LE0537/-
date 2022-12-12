#include "stdafx.h"
#include "..\Public\Tackle.h"

#include "GameInstance.h"

CTackle::CTackle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CTackle::CTackle(const CTackle & rhs)
	: CGameObj(rhs)
{
}

HRESULT CTackle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTackle::Initialize(void * pArg)
{

//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("몸통 박치기");
	m_SkillInfo.strInfo = TEXT("상대를 향해서 몸 전체를 던져 공격한다.");
	m_SkillInfo.iSkillNum = 0;
	m_SkillInfo.iDmg = 40;
	m_SkillInfo.iSDmg = 0;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 35;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = NORMAL;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CTackle::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (m_fSkillTime > 1.f)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tackle2"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
				return;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tackle3"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
			m_fSkillTime = 0.f;
			m_SkillInfo.bUseSkill = false;
		}
	}
}

void CTackle::Late_Tick(_float fTimeDelta)
{

}

HRESULT CTackle::Render()
{
	return S_OK;
}

CTackle * CTackle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTackle*	pInstance = new CTackle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTackle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTackle::Clone(void * pArg)
{
	CGameObject*	pInstance = new CTackle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTackle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTackle::Free()
{
	__super::Free();

}
