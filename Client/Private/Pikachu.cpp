#include "stdafx.h"
#include "..\Public\Pikachu.h"
#include "GameInstance.h"


CPikachu::CPikachu(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPikachu::CPikachu(const CPikachu & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPikachu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPikachu::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	m_PokemonInfo.strName = TEXT("피카츄");
	m_PokemonInfo.strInfo = TEXT("볼에 전기를 저장하는 주머니가 있다. 숲을 거처로 삼으며,\n 단단한 나무열매를 전기로 익혀 먹는 등의 지혜를 가졌다.");
	m_PokemonInfo.strChar = TEXT("온순함");
	m_PokemonInfo.iPokeNum = 25;
	m_PokemonInfo.iMaxHp = 200;
	m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp;
	m_PokemonInfo.iDmg = 70;
	m_PokemonInfo.iSDmg = 100;
	m_PokemonInfo.iDef = 50;
	m_PokemonInfo.iSDef = 70;
	m_PokemonInfo.iSpeed = 140;
	m_PokemonInfo.iLv = 5;
	m_PokemonInfo.iMaxExp = 20;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iSex = 0;
	m_PokemonInfo.iBallNum = 0;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tackle"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum1)))
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

	

	m_PokemonInfo.eType = THUNDER;
	m_PokemonInfo.eType2 = POKETYPE_END;
	m_PokemonInfo.eStatInfo = STATINFO_END;



	return S_OK;
}

void CPikachu::Tick(_float fTimeDelta)
{
	int ia = m_PokemonInfo.eItem->iItemNum;
	int ib = m_PokemonInfo.eSkillNum3->iSkillNum;
	int ic = 0;
}

void CPikachu::Late_Tick(_float fTimeDelta)
{

}

HRESULT CPikachu::Render()
{
	return S_OK;
}

CPikachu * CPikachu::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPikachu*	pInstance = new CPikachu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPikachu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPikachu::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPikachu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPikachu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPikachu::Free()
{
	__super::Free();

}
