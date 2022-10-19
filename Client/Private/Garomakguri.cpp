#include "stdafx.h"
#include "..\Public\Garomakguri.h"
#include "GameInstance.h"

CGaromakguri::CGaromakguri(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CGaromakguri::CGaromakguri(const CGaromakguri & rhs)
	: CGameObj(rhs)
{
}

HRESULT CGaromakguri::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGaromakguri::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	m_PokemonInfo.strName = TEXT("해월가로막구리");
	m_PokemonInfo.strInfo = TEXT("쥬신 130기 배해월, 난폭운전으로 유명하다.");
	m_PokemonInfo.strChar = TEXT("난폭");
	m_PokemonInfo.iPokeNum = 152;
	m_PokemonInfo.iMaxHp = 250;
	m_PokemonInfo.iHp = 30;
	m_PokemonInfo.iDmg = 130;
	m_PokemonInfo.iSDmg = 180;
	m_PokemonInfo.iDef = 150;
	m_PokemonInfo.iSDef = 150;
	m_PokemonInfo.iSpeed = 130;
	m_PokemonInfo.iLv = 5;
	m_PokemonInfo.iMaxExp = 20;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iSex = 1;
	m_PokemonInfo.iBallNum = 2;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tackle"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum1)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BreakCar"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum2)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BreakCar"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum3)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BreakCar"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum4)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_None"), LEVEL_STATIC, TEXT("Layer_Item"), &m_PokemonInfo.eItem)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	m_PokemonInfo.eType = EVIL;
	m_PokemonInfo.eType2 = NORMAL;
	m_PokemonInfo.eStatInfo = BURN;


	return S_OK;
}

void CGaromakguri::Tick(_float fTimeDelta)
{

}

void CGaromakguri::Late_Tick(_float fTimeDelta)
{

}

HRESULT CGaromakguri::Render()
{
	return S_OK;
}

CGaromakguri * CGaromakguri::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGaromakguri*	pInstance = new CGaromakguri(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CGaromakguri"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CGaromakguri::Clone(void * pArg)
{
	CGameObject*	pInstance = new CGaromakguri(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CGaromakguri"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGaromakguri::Free()
{
	__super::Free();

}
