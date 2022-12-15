#include "stdafx.h"
#include "..\Public\Scissor.h"

#include "GameInstance.h"

CScissor::CScissor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CScissor::CScissor(const CScissor & rhs)
	: CGameObj(rhs)
{
}

HRESULT CScissor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScissor::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("시저크로스");
	m_SkillInfo.strInfo = TEXT("낫이나 발톱을 가위처럼 교차시키며\n상대를 베어 가른다.");
	m_SkillInfo.iSkillNum = 10;
	m_SkillInfo.iDmg = 80;
	m_SkillInfo.iSDmg = 0;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 15;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = BUG;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CScissor::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (!m_bSkill && m_fSkillTime > 1.f)
		{
			Set_Pos(fTimeDelta);
			m_bSkill = true;
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

void CScissor::Late_Tick(_float fTimeDelta)
{

}

HRESULT CScissor::Render()
{
	return S_OK;
}

void CScissor::Set_Pos(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Scissor1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
	
}

CScissor * CScissor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CScissor*	pInstance = new CScissor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CScissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CScissor::Clone(void * pArg)
{
	CGameObject*	pInstance = new CScissor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CScissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScissor::Free()
{
	__super::Free();

}
