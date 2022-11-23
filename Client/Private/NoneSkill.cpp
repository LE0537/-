#include "stdafx.h"
#include "..\Public\NoneSkill.h"

#include "GameInstance.h"

CNoneSkill::CNoneSkill(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CNoneSkill::CNoneSkill(const CNoneSkill & rhs)
	: CGameObj(rhs)
{
}

HRESULT CNoneSkill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNoneSkill::Initialize(void * pArg)
{

	//*(CGameObject**)pArg = this;


	m_SkillInfo.strName = TEXT("");
	m_SkillInfo.strInfo = TEXT("");
	m_SkillInfo.iSkillNum = 99;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 0;
	m_SkillInfo.fHit = 0;
	m_SkillInfo.iMaxPoint = 0;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = POKETYPE_END;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CNoneSkill::Tick(_float fTimeDelta)
{

}

void CNoneSkill::Late_Tick(_float fTimeDelta)
{

}

HRESULT CNoneSkill::Render()
{
	return S_OK;
}

CNoneSkill * CNoneSkill::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNoneSkill*	pInstance = new CNoneSkill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CNoneSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CNoneSkill::Clone(void * pArg)
{
	CGameObject*	pInstance = new CNoneSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CNoneSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNoneSkill::Free()
{
	__super::Free();

}
