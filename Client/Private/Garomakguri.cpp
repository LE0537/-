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

	m_PokemonInfo.strName = TEXT("�ؿ����θ�����");
	m_PokemonInfo.strInfo = TEXT("��� 130�� ���ؿ�, ������������ �����ϴ�.");
	m_PokemonInfo.iPokeNum = 152;
	m_PokemonInfo.iMaxHp = 100;
	m_PokemonInfo.iHp = 30;
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
	m_PokemonInfo.iBallNum = 2;
	m_PokemonInfo.eSkillNum1.iSkillNum = 0;
	m_PokemonInfo.eSkillNum2.iSkillNum = 98;
	m_PokemonInfo.eSkillNum3.iSkillNum = 98;
	m_PokemonInfo.eSkillNum4.iSkillNum = 98;

	m_PokemonInfo.eSkillNum1.strName = TEXT("���� ��ġ��");
	m_PokemonInfo.eSkillNum1.strInfo = TEXT("��븦 ���ؼ� �� ��ü�� ���� �����Ѵ�.");
	m_PokemonInfo.eSkillNum1.iSkillNum = 0;
	m_PokemonInfo.eSkillNum1.iDmg = 40;
	m_PokemonInfo.eSkillNum1.iSDmg = 0;
	m_PokemonInfo.eSkillNum1.fHit = 100;
	m_PokemonInfo.eSkillNum1.iMaxPoint = 35;
	m_PokemonInfo.eSkillNum1.iPoint = m_PokemonInfo.eSkillNum1.iMaxPoint;
	m_PokemonInfo.eSkillNum1.eType = NORMAL;

	m_PokemonInfo.eSkillNum2.strName = TEXT("��������");
	m_PokemonInfo.eSkillNum2.strInfo = TEXT("���ؿ� Ư�� �������� �̴�.");
	m_PokemonInfo.eSkillNum2.iSkillNum = 0;
	m_PokemonInfo.eSkillNum2.iDmg = 0;
	m_PokemonInfo.eSkillNum2.iSDmg = 150;
	m_PokemonInfo.eSkillNum2.fHit = 50;
	m_PokemonInfo.eSkillNum2.iMaxPoint = 5;
	m_PokemonInfo.eSkillNum2.iPoint = m_PokemonInfo.eSkillNum2.iMaxPoint;
	m_PokemonInfo.eSkillNum2.eType = EVIL;


	m_PokemonInfo.eSkillNum3.strName = TEXT("��������");
	m_PokemonInfo.eSkillNum3.strInfo = TEXT("���ؿ� Ư�� �������� �̴�.");
	m_PokemonInfo.eSkillNum3.iSkillNum = 0;
	m_PokemonInfo.eSkillNum3.iDmg = 0;
	m_PokemonInfo.eSkillNum3.iSDmg = 150;
	m_PokemonInfo.eSkillNum3.fHit = 50;
	m_PokemonInfo.eSkillNum3.iMaxPoint = 5;
	m_PokemonInfo.eSkillNum3.iPoint = m_PokemonInfo.eSkillNum3.iMaxPoint;
	m_PokemonInfo.eSkillNum3.eType = EVIL;

	m_PokemonInfo.eSkillNum4.strName = TEXT("��������");
	m_PokemonInfo.eSkillNum4.strInfo = TEXT("���ؿ� Ư�� �������� �̴�.");
	m_PokemonInfo.eSkillNum4.iSkillNum = 0;
	m_PokemonInfo.eSkillNum4.iDmg = 0;
	m_PokemonInfo.eSkillNum4.iSDmg = 150;
	m_PokemonInfo.eSkillNum4.fHit = 50;
	m_PokemonInfo.eSkillNum4.iMaxPoint = 5;
	m_PokemonInfo.eSkillNum4.iPoint = m_PokemonInfo.eSkillNum4.iMaxPoint;
	m_PokemonInfo.eSkillNum4.eType = EVIL;

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
