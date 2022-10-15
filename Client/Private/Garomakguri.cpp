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

	m_PokemonInfo.strName = TEXT("가로막구리");
	m_PokemonInfo.strInfo = TEXT("쥬신 130기 배해월, 난폭운전으로 유명하다.");
	m_PokemonInfo.iPokeNum = 152;
	m_PokemonInfo.iMaxHp = 100;
	m_PokemonInfo.iHp = 0;
	m_PokemonInfo.iDmg = 30;
	m_PokemonInfo.iSDmg = 30;
	m_PokemonInfo.iDef = 10;
	m_PokemonInfo.iSDef = 10;
	m_PokemonInfo.iSpeed = 30;
	m_PokemonInfo.iLv = 5;
	m_PokemonInfo.iMaxExp = 20;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iItem = 99;
	m_PokemonInfo.iSex = 1;

	m_PokemonInfo.eType = EVIL;
	m_PokemonInfo.eType2 = NORMAL;
	m_PokemonInfo.eStatInfo = STUN;


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
