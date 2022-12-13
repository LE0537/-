#include "stdafx.h"
#include "..\Public\WaterGun.h"

#include "GameInstance.h"

CWaterGun::CWaterGun(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWaterGun::CWaterGun(const CWaterGun & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWaterGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterGun::Initialize(void * pArg)
{

	//	*(CGameObject**)pArg = this;
	m_SkillInfo.strName = TEXT("물대포");
	m_SkillInfo.strInfo = TEXT("강력한 물폭탄을 발사한다.");
	m_SkillInfo.iSkillNum = 2;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 40;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 25;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = WATER;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;
	return S_OK;
}

void CWaterGun::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		m_fSkillTime += fTimeDelta;
		if (m_fSkillTime > 0.7f)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WaterGun1"), LEVEL_GAMEPLAY, TEXT("Effect"), &m_SkillInfo)))
				return;
		

			RELEASE_INSTANCE(CGameInstance);
			m_fSkillTime = 0.f;
			m_SkillInfo.bUseSkill = false;
		}
	}
}

void CWaterGun::Late_Tick(_float fTimeDelta)
{

}

HRESULT CWaterGun::Render()
{
	return S_OK;
}

CWaterGun * CWaterGun::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterGun*	pInstance = new CWaterGun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWaterGun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWaterGun::Clone(void * pArg)
{
	CGameObject*	pInstance = new CWaterGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWaterGun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterGun::Free()
{
	__super::Free();

}
