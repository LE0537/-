#include "stdafx.h"
#include "..\Public\NonePoke.h"
#include "GameInstance.h"

CNonePoke::CNonePoke(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CNonePoke::CNonePoke(const CNonePoke & rhs)
	: CGameObj(rhs)
{
}

HRESULT CNonePoke::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNonePoke::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	m_PokemonInfo.strName = TEXT("");
	m_PokemonInfo.strInfo = TEXT("");
	m_PokemonInfo.strChar = TEXT("");
	m_PokemonInfo.iPokeNum = 999;
	m_PokemonInfo.iMaxHp = 100;
	m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp;
	m_PokemonInfo.iDmg = 30;
	m_PokemonInfo.iSDmg = 30;
	m_PokemonInfo.iDef = 10;
	m_PokemonInfo.iSDef = 10;
	m_PokemonInfo.iSpeed = 30;
	m_PokemonInfo.iLv = 5;
	m_PokemonInfo.iMaxExp = 20;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iSex = 99;
	m_PokemonInfo.iBallNum = 99;
	m_PokemonInfo.bRide = false;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NoneSkill"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum1)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NoneSkill"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum2)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NoneSkill"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum3)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NoneSkill"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum4)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_None"), LEVEL_STATIC, TEXT("Layer_Item"), &m_PokemonInfo.eItem)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	m_PokemonInfo.eType = POKETYPE_END;
	m_PokemonInfo.eType2 = POKETYPE_END;
	m_PokemonInfo.eStatInfo = STATINFO_END;
	m_PokemonInfo.bEvolution = false;

	return S_OK;
}

void CNonePoke::Tick(_float fTimeDelta)
{

}

void CNonePoke::Late_Tick(_float fTimeDelta)
{

}

HRESULT CNonePoke::Render()
{
	return S_OK;
}

HRESULT CNonePoke::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CNonePoke * CNonePoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNonePoke*	pInstance = new CNonePoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CNonePoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CNonePoke::Clone(void * pArg)
{
	CGameObject*	pInstance = new CNonePoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CNonePoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNonePoke::Free()
{
	__super::Free();

}
